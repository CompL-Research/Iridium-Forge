#include "Storage/IridiumPool.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumMeta.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace IRI_STORAGE {

//
// Pool operations
//
IRID IridiumPool::add_node(IRI_GEN::IRI_TAG tag, const std::vector<IRID> &args,
                           const std::vector<FlagValue> &flags) {
  IridiumSEXP node;
  node.tag = tag;

  // 1. Pack Args
  node.num_args = static_cast<IRID>(args.size());
  node.args_start_index = static_cast<IRID>(args_pool.size());
  args_pool.insert(args_pool.end(), args.begin(), args.end());

  // 2. Pack Flags
  uint32_t expectedNumSlots = IRI_GEN::IridiumMeta::get_flag_slots(tag);
  if (expectedNumSlots != flags.size())
    throw std::runtime_error("[Forge-IridiumPool]: " + IRI_GEN::dump_tag(tag) +
                             " expects " + std::to_string(expectedNumSlots) +
                             " flags, provided " +
                             std::to_string(flags.size()));

  node.num_flag_slots = static_cast<IRID>(flags.size());

  if (flags.empty()) {
    node.flags_start_index = 0;
  } else {
    // Check if this exact sequence of flags already exists
    auto it = flag_intern_map.find(flags);

    if (it != flag_intern_map.end()) {
      // REUSE: Point to the existing block in the pool
      node.flags_start_index = it->second;
    } else {
      // ALLOCATE: Append to the pool and save the location
      node.flags_start_index = static_cast<IRID>(flags_pool.size());

      // --- DEBUG BLOCK ---
      if (flags.size() > flags_pool.max_size() - flags_pool.size()) {
        std::cerr << "[CRITICAL ERROR] Vector length limit reached!"
                  << std::endl;
        std::cerr << "Current pool size: " << flags_pool.size() << std::endl;
        std::cerr << "Attempting to add: " << flags.size() << " elements"
                  << std::endl;
        std::cerr << "Vector max_size:   " << flags_pool.max_size()
                  << std::endl;
        throw std::runtime_error("[Forge-IridiumPool]: Pool limit reached");
      }

      // Check for "Garbage" sizes (e.g., if tag metadata is corrupted)
      if (flags.size() > 0xFFFFFF) {
        std::cerr << "[WARNING] Extremely large flag count detected for tag: "
                  << IRI_GEN::dump_tag(tag) << " (" << flags.size() << ")"
                  << std::endl;
        throw std::runtime_error("[Forge-IridiumPool]: Likely invalid flags");
      }

      flags_pool.insert(flags_pool.end(), flags.begin(), flags.end());
      flag_intern_map[flags] = node.flags_start_index;
    }
  }

  // Add
  nodes.push_back(node);
  return static_cast<IRID>(nodes.size() - 1);
}

//
// Node operations
//
const IridiumSEXP &IridiumPool::get_node(IRID id) const { return nodes[id]; }

void IridiumPool::update_tag(IRID id, IRI_GEN::IRI_TAG tag) {
  nodes[id].tag = tag;
}

std::span<const IRID> IridiumPool::get_args_view(const IridiumSEXP *n) const {
  return {args_pool.data() + n->args_start_index, n->num_args};
}

std::span<const IRID> IridiumPool::get_args_view(IRID id) const {
  const auto &node = nodes[id];
  return {args_pool.data() + node.args_start_index, node.num_args};
}

std::vector<IRID> IridiumPool::get_args(const IridiumSEXP *n) {
  auto start_iter = args_pool.begin() + n->args_start_index;
  return std::vector<IRID>(start_iter, start_iter + n->num_args);
}

std::vector<IRID> IridiumPool::get_args(IRID id) {
  const auto &node = nodes[id];
  auto start_iter = args_pool.begin() + node.args_start_index;
  return std::vector<IRID>(start_iter, start_iter + node.num_args);
}

std::span<const FlagValue> IridiumPool::get_flags(const IridiumSEXP *n) const {
  return {flags_pool.data() + n->flags_start_index, n->num_flag_slots};
}

std::span<const FlagValue> IridiumPool::get_flags(IRID id) const {
  const auto &node = nodes[id];
  return {flags_pool.data() + node.flags_start_index, node.num_flag_slots};
}

//
// Mutation
//

std::span<FlagValue> IridiumPool::get_flags_m(IRID id) {
  auto &node = nodes[id];
  if (mutatedFlags.contains(id)) {
    return {flags_pool.data() + node.flags_start_index, node.num_flag_slots};
  }

  mutatedFlags.insert(id);

  // Copy the current shared flags to the end of the pool
  IRID new_start = static_cast<IRID>(flags_pool.size());
  auto old_begin = flags_pool.begin() + node.flags_start_index;
  auto old_end = old_begin + node.num_flag_slots;

  flags_pool.insert(flags_pool.end(), old_begin, old_end);

  // Update the node to point to this new, unique, private block
  node.flags_start_index = new_start;

  // Return a mutable span to the new private block
  return {flags_pool.data() + node.flags_start_index, node.num_flag_slots};
}

void IridiumPool::set_args(IRID id, const std::vector<IRID> &target) {
  auto &node = nodes[id];

  if (target.size() <= node.num_args) {
    node.num_args = target.size();
    for (size_t i = 0; i < target.size(); i++) {
      args_pool[node.args_start_index + i] = target[i];
    }
  } else {
    node.num_args = static_cast<IRID>(target.size());
    node.args_start_index = static_cast<IRID>(args_pool.size());
    args_pool.insert(args_pool.end(), target.begin(), target.end());
  }
}

void IridiumPool::update_arg_inplace(IRID id, IRID offset, IRID target) {
  auto &node = nodes[id];
  args_pool[node.args_start_index + offset] = target;
}

void IridiumPool::poke_out_arg_at_index(IRID id, IRID offset) {
  auto &node = nodes[id];
  if (offset >= node.num_args)
    throw std::runtime_error("poke_out_arg_at_index undeflow");

  if (offset == 0) {
    node.args_start_index++;
  } else {
    // 1. Create a bounded view of ONLY this node's arguments
    std::span node_args(&args_pool[node.args_start_index], node.num_args);

    // 2. Shift the tail elements left by 1 position to overwrite the target
    std::ranges::move(node_args.subspan(offset + 1),
                      node_args.begin() + offset);
  }

  node.num_args--;
}

void IridiumPool::add_args_to_beginning(IRID id,
                                        const std::vector<IRID> &args) {
  auto &node = nodes[id];
  if (args.empty())
    return;

  args_pool.reserve(args_pool.size() + args.size() + node.num_args);
  std::span old_args(args_pool.begin() + node.args_start_index, node.num_args);
  node.args_start_index = args_pool.size();
  node.num_args += args.size();
  std::ranges::copy(args, std::back_inserter(args_pool));
  std::ranges::copy(old_args, std::back_inserter(args_pool));
}

void IridiumPool::add_args_to_end(IRID id, const std::vector<IRID> &args) {
  auto &node = nodes[id];
  if (args.empty())
    return;

  // In-place optimization: If we are already at the end, just append.
  if (node.args_start_index + node.num_args == args_pool.size()) {
    std::ranges::copy(args, std::back_inserter(args_pool));
    node.num_args += args.size();
    return;
  }

  // Otherwise, relocate. Reserve first!
  args_pool.reserve(args_pool.size() + node.num_args + args.size());

  std::span old_args(args_pool.begin() + node.args_start_index, node.num_args);
  node.args_start_index = args_pool.size();
  node.num_args += args.size();

  std::ranges::copy(old_args, std::back_inserter(args_pool));
  std::ranges::copy(args, std::back_inserter(args_pool));
}

void IridiumPool::remove_args_matching_tag(IRID id, IRI_GEN::IRI_TAG tag) {
  auto &node = nodes[id];
  if (node.num_args == 0)
    return;

  std::span node_args(args_pool.begin() + node.args_start_index, node.num_args);

  auto [removed_begin, removed_end] = std::ranges::remove_if(
      node_args, [tag](auto node_tag) { return node_tag == tag; }, // Predicate
      [this](IRID arg_id) { return nodes[arg_id].tag; }            // Projection
  );

  node.num_args = std::distance(node_args.begin(), removed_begin);
}

void IridiumPool::update_num_args(IRID id, uint32_t num_args) {
  auto &node = nodes[id];
  if (node.num_args == num_args) // no change
    return;

  if (node.num_args < num_args)
    throw std::runtime_error("update_num_args is only expected to shrink args "
                             "inplace, not extend it!");

  node.num_args = num_args;
}

IridiumPoolTelemetry IridiumPool::telemetry() const {
  auto stat = [](const auto &v) {
    using T = typename std::decay_t<decltype(v)>::value_type;
    return PoolMemStats{v.size(), v.size() * sizeof(T),
                        v.capacity() * sizeof(T)};
  };
  return {stat(nodes), stat(args_pool), stat(flags_pool)};
}

} // namespace IRI_STORAGE
