#include "Storage/IridiumPool.h"
#include "Storage/Config.h"
#include "Support/IRIS.hpp"
#include <iostream>
#if DUMP_CORE_PASSES == 1
#include <fstream>
#endif

namespace IRI_STORAGE {

std::vector<IRID> IridiumPool::collect_roots(IRID entry) const {
  std::vector<IRID> roots = {
    entry,
    NULL_SEXP, NOP_SEXP, NUBD_SEXP, TRUE_SEXP, FALSE_SEXP
  };

  // if IRIS exists, put referenced IRIDS in root set
  if (iris) {
    auto iris_roots = iris->collect_referenced_irids();
    roots.insert(roots.end(), iris_roots.begin(), iris_roots.end());
  }

  return roots;
}

ReachabilityResult IridiumPool::mark_reachable(std::span<const IRID> roots) const {
  ReachabilityResult result;
  result.visited.assign(nodes.size(), false);

  if (nodes.empty()) return result;

  // worklist works like a stack
  std::vector<IRID> worklist;

  for (IRID node : roots) {
    if (!result.visited[node]) {
      result.visited[node] = true;
      worklist.push_back(node);
      result.reachable_count++;
    }
  }

  // visit a node, mark it reachable, go to children (depth-first) and mark accordingly
  while (!worklist.empty()) {
    IRID id = worklist.back();
    worklist.pop_back();

    const IridiumSEXP &node = nodes[id];
    for (IRID child : get_args_view(&node)) {
      if (!result.visited[child]) {
        result.visited[child] = true;
        worklist.push_back(child);
        result.reachable_count++;
      }
    }
  }

  return result;
}

void IridiumPool::print_reachability(const ReachabilityResult &reach) const {
  size_t total = nodes.size();
  size_t reachable = reach.reachable_count;
  size_t unreachable = total - reachable;
  double garbage_pct = total > 0
      ? (static_cast<double>(unreachable) / total) * 100.0
      : 0.0;

  std::cout << "\n------- IridiumPool Reachability --------\n";
  std::cout << "Total nodes:      " << total << std::endl;
  std::cout << "Reachable:        " << reachable << std::endl;
  std::cout << "Unreachable:      " << unreachable << std::endl;
  std::cout << "Garbage %:        " << garbage_pct << "%" << std::endl;

  std::cout << "\n-------- Reachable nodes -------------\n";
  for (size_t i = 0; i < reach.visited.size(); i++) {
    if (reach.visited[i] == true) {
      std::cout << "[" << i << "], ";
    }
  }
  std::cout << std::endl;
}

CompactionResult IridiumPool::compact_pool(ReachabilityResult &reach)
{
  CompactionResult result;
  // initialize all IRID mappings to -1
  // at last -1 means that is unreachable
  result.old_to_new_irid_map.assign(nodes.size(), static_cast<IRID>(-1));

  // create a copy of nodes containing only the reachable IridiumSEXPs
  std::vector<IridiumSEXP> new_nodes;
  new_nodes.reserve(reach.reachable_count);

  // count the total number of args from all the reachable nodes
  size_t reachable_args_count = 0;

  // count an upper bound of the number of flag slots needed for the reachable nodes
  size_t reachable_flags_upper_bound = 0;

  // Pass 1
  // map old IRIDs to new IRIDs in the new_nodes
  for (size_t old_id = 0; old_id < nodes.size(); old_id++)
  {
    if (reach.visited[old_id])
    {
      result.old_to_new_irid_map[old_id] = static_cast<IRID>(new_nodes.size());
      new_nodes.push_back(nodes[old_id]);
      reachable_args_count += nodes[old_id].num_args;
      reachable_flags_upper_bound += nodes[old_id].num_flag_slots;
    }
  }

  std::vector<IRID> new_args_pool;
  new_args_pool.reserve(reachable_args_count);

  std::vector<FlagValue> new_flags_pool;
  new_flags_pool.reserve(reachable_flags_upper_bound);

  std::unordered_map<std::vector<FlagValue>, uint32_t, FlagVectorHash, FlagVectorEq> new_flag_intern_map;

  // Pass 2
  // Walk new_nodes list and rewrite each node's args
  // Also compacts the flags_pool
  for (size_t new_id = 0; new_id < new_nodes.size(); new_id++)
  {
    IridiumSEXP &node = new_nodes[new_id];

    // Copying to new_args_pool
    uint32_t start_index = node.args_start_index;
    node.args_start_index = new_args_pool.size();

    for (uint32_t offset = 0; offset < node.num_args; offset++)
    {
      IRID old_child_irid = args_pool[start_index + offset];
      IRID new_child_irid = result.old_to_new_irid_map[old_child_irid];
      new_args_pool.push_back(new_child_irid);
    }

    // Do flags_pool compaction
    if (node.num_flag_slots == 0)
    {
      node.flags_start_index = 0;
      continue;
    }

    uint32_t old_flags_start = node.flags_start_index;
    std::vector<FlagValue> flags_content(
        flags_pool.begin() + old_flags_start,
        flags_pool.begin() + old_flags_start + node.num_flag_slots);

    // check whether the flags flags_content is already present in the new_flags_pool
    auto it = new_flag_intern_map.find(flags_content);
    if (it != new_flag_intern_map.end())
    { // flags_content found, reuse it
      node.flags_start_index = it->second;
    }
    else
    { // flags_content not found, put into new_flags_pool and in the intern map
      uint32_t new_flags_start = static_cast<uint32_t>(new_flags_pool.size());
      new_flags_pool.insert(new_flags_pool.end(), flags_content.begin(), flags_content.end());

      // Record it in the map for future nodes
      new_flag_intern_map[flags_content] = new_flags_start;
      node.flags_start_index = new_flags_start;
    }
  }

  // commit storage copy to actual pool
  nodes = std::move(new_nodes);
  args_pool = std::move(new_args_pool);
  flags_pool = std::move(new_flags_pool);
  flags_pool.shrink_to_fit();

  flag_intern_map = std::move(new_flag_intern_map);

  // mutatedFlags cleared to avoid assuming private flag block after re-interning
  mutatedFlags.clear();

  return result;
}

void IridiumPool::print_compaction_result(const CompactionResult &compaction) const {
  size_t initial_nodes_size = compaction.old_to_new_irid_map.size();
  size_t compacted_nodes_size = nodes.size();
  double garbage_pct = initial_nodes_size > 0
                           ? (static_cast<double>(initial_nodes_size - compacted_nodes_size) / initial_nodes_size) * 100.0
                           : 0.0;

  std::cout << "Nodes before GC:      " << initial_nodes_size << std::endl;
  std::cout << "Nodes after GC:       " << compacted_nodes_size << std::endl;
  std::cout << "Garbage collected:    " << garbage_pct << " %" << std::endl;
  std::cout << "Args pool size:       " << args_pool.size() << std::endl;
  std::cout << "Flags pool size:      " << flags_pool.size() << std::endl;
}

CompactionResult IridiumPool::garbage_collect(IRID &root, const std::string &label)
{
  std::vector<IRID> roots = collect_roots(root);
  auto reach = mark_reachable(roots);

#if DUMP_CORE_PASSES == 1
  std::cout << "\n"
            << label << " :\n";
  print_reachability(reach);

  std::cout << "\nArgs_pool size after iris commit: " << args_pool.size() << "\n";
  std::cout << "\nFlags_pool size after iris commit: " << flags_pool.size() << "\n";

  {
    std::ofstream out(label + "_with_ids.iridump");
    (*this)[root].dumpFlat(out, this, root);
  }
#endif

  auto compaction_result = compact_pool(reach);
  root = compaction_result.old_to_new_irid_map[root];
  if (iris)
  {
    iris->remap_irids(compaction_result.old_to_new_irid_map);
  }

#if DUMP_CORE_PASSES == 1
  std::cout << "\n-------- " + label + " Garbage Collection --------\n";
  print_compaction_result(compaction_result);
#endif

  return compaction_result;
}

}