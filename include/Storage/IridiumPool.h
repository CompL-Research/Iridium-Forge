#pragma once
#include "Config.h"
#include "FlagInterning.h"
#include "Generated/IridiumEnums.h"
#include "IridiumSEXP.h"
#include "StringPool.h"
#include <cstdint>
#include <memory>
#include <optional>
#include <set>
#include <span>
#include <unordered_map>
#include <vector>

namespace IRI_STRUCTURAL {
  class IRIS;
  class ClosureTree;
}

namespace IRI_STORAGE {

// store result of Reachability analysis for doing GC
// each index in visited tells whether node at that IRID is reachable
struct ReachabilityResult {
  std::vector<bool> visited;  // vector indexed by IRID for nodes
  size_t reachable_count = 0; // total number of reachable nodes
};

// store old -> new IRID mappings
struct CompactionResult {
  // index: old IRID, element: new IRID
  // old_to_new_irid_map[old_irid] = new_irid
  std::vector<IRID> old_to_new_irid_map;
};

// pool memory statistics
struct PoolMemory {
  size_t nodeCount = 0, argCount = 0, flagCount = 0, flagInternEntries = 0;
  size_t nodesB = 0, argsB = 0, flagsB = 0, flagInternB = 0;  // size in bytes
  size_t nodesCapB = 0, argsCapB = 0, flagsCapB = 0;  // capacity in bytes
  size_t usedBytes = 0, capBytes = 0;   // total
};

class IridiumPool {

public:
  IRID NULL_SEXP;
  IRID NOP_SEXP;
  IRID NUBD_SEXP;
  IRID TRUE_SEXP;
  IRID FALSE_SEXP;

  double lastBBIDX = 0;

  // Must be empty after stack->heap pass
  // added here as creating a new flag just for this is very memory
  // hungry
  std::unordered_map<IRID, double> CONTINUE_TARGETS;

  StringID getTemp();

  std::shared_ptr<IRI_STRUCTURAL::IRIS> iris = nullptr;
  std::shared_ptr<IRI_STRUCTURAL::ClosureTree> closureTree = nullptr;

  //
  // String interning
  //
  StringPool strings;

  //
  // Pool operations
  //

  // Add a new node to the pool and return its IRID
  IRID add_node(IRI_GEN::IRI_TAG tag, const std::vector<IRID> &args,
                const std::vector<FlagValue> &flags);

  //
  // Node operations
  //

  // Given an IRID, retrieve a node
  const IridiumSEXP &operator[](IRID id) const;
  void update_tag(IRID id, IRI_GEN::IRI_TAG);

  // Given an IRID/IridiumSEXP return its args (immutable)
  std::span<const IRID> get_args_view(const IridiumSEXP *n) const;
  std::span<const IRID> get_args_view(IRID id) const;

  // Safe when new args may be added/removed
  std::vector<IRID> get_args(const IridiumSEXP *n);
  std::vector<IRID> get_args(IRID id);


  // Given an IRID/IridiumSEXP return its flags (immutable)
  std::span<const FlagValue> get_flags(const IridiumSEXP *n) const;
  std::span<const FlagValue> get_flags(IRID id) const;

  //
  // Mutation
  //

  // Flags for mutation (sets a flag mutatedFlags to avoid recopying)
  std::span<FlagValue> get_flags_m(IRID id);

  // Update args for a given IRID,
  // args.size <= get_args(id) -> Mutate in place
  // copy args to a new location and update node to new targets
  void set_args(IRID id, const std::vector<IRID> &args);

  // Update arg inplace
  void update_arg_inplace(IRID id, IRID offset, IRID target);

  void poke_out_arg_at_index(IRID id, IRID offset);

  void add_args_to_beginning(IRID id, const std::vector<IRID> &args);

  void add_args_to_end(IRID id, const std::vector<IRID> &args);

  void remove_args_matching_tag(IRID id, IRI_GEN::IRI_TAG tag);

  void update_num_args(IRID id, uint32_t num_args);

  // collect roots to start reachability
  std::vector<IRID> collect_roots(IRID entry) const;

  // Reachability
  // traverse nodes starting from the root (DFS) and mark them as reachable
  ReachabilityResult mark_reachable(std::span<const IRID> roots) const;

  // print reachability result
  void print_reachability(const ReachabilityResult &reach) const;

  // args_pool size
  size_t get_args_pool_size() const;

  // flags_pool size
  size_t get_flags_pool_size() const;

  // use Reachability result to compact nodes and args_pool
  CompactionResult compact_pool(ReachabilityResult &reach);

  // print compaction result
  void print_compaction_result(const std::string &label, const PoolMemory &before, const PoolMemory &after, double ms) const;

  // wrapper for both reachability and compaction
  CompactionResult garbage_collect(IRID &root, const std::string &label);

  // get pool memory stats
  PoolMemory memory_usage() const;

  ~IridiumPool();
  IridiumPool() = default;
  IridiumPool(const IridiumPool &) = delete;
  IridiumPool &operator=(const IridiumPool &) = delete;

private:
  std::vector<IridiumSEXP> nodes;
  std::vector<IRID> args_pool;
  std::vector<FlagValue> flags_pool;
  std::unordered_map<std::vector<FlagValue>, uint32_t, FlagVectorHash,
                     FlagVectorEq>
      flag_intern_map;
  std::set<IRID> mutatedFlags;
  uint32_t tempIDX = 0;
};

} // namespace IRI_STORAGE
