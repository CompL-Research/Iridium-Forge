#pragma once
#include "Config.h"
#include "FlagInterning.h"
#include "Generated/IridiumEnums.h"
#include "IridiumSEXP.h"
#include "PoolMemStats.h"
#include <cstddef>
#include <cstdint>
#include <set>
#include <span>
#include <unordered_map>
#include <vector>

namespace IRI_STORAGE {

class IRIContext;

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
  
struct PoolMemory {
  size_t nodeCount = 0, argCount = 0, flagCount = 0, flagInternEntries = 0;
  size_t nodesB = 0, argsB = 0, flagsB = 0, flagInternB = 0;  // size in bytes
  size_t nodesCapB = 0, argsCapB = 0, flagsCapB = 0;  // capacity in bytes
  size_t usedBytes = 0, capBytes = 0;   // total
};

struct IridiumPoolTelemetry {
  PoolMemStats nodes, args, flags;
};

class IridiumPool {

public:
  //
  // Shared Globals
  //
  IRID NULL_SEXP;
  IRID NOP_SEXP;
  IRID NUBD_SEXP;

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
  const IridiumSEXP &get_node(IRID id) const;
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
  // TODO: I never got to improving these methods, maybe someday... ~Meetesh
  void update_arg_inplace(IRID id, IRID offset, IRID target);

  void poke_out_arg_at_index(IRID id, IRID offset);

  void add_args_to_beginning(IRID id, const std::vector<IRID> &args);

  void add_args_to_end(IRID id, const std::vector<IRID> &args);

  void remove_args_matching_tag(IRID id, IRI_GEN::IRI_TAG tag);

  void update_num_args(IRID id, uint32_t num_args);

  // collect roots to start reachability
  std::vector<IRID> collectRoots(IRIContext &ctx, IRID entry) const;

  // Reachability
  // traverse nodes starting from the root (DFS) and mark them as reachable
  ReachabilityResult markReachable(std::span<const IRID> roots) const;

  // print reachability result
  void printReachability(const ReachabilityResult &reach) const;

  // args_pool size
  size_t get_args_pool_size() const;

  // flags_pool size
  size_t get_flags_pool_size() const;

  // use Reachability result to compact nodes and args_pool
  CompactionResult compactPool(ReachabilityResult &reach);

  // print compaction result
  void printCompactionResult(const std::string &label, const PoolMemory &before, const PoolMemory &after, double ms) const;

  // wrapper for both reachability and compaction
  CompactionResult garbageCollect(IRIContext &ctx, IRID &root, const std::string &label);

  // get pool memory stats
  PoolMemory memoryUsage() const;
  
  IridiumPoolTelemetry telemetry() const;

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
};

} // namespace IRI_STORAGE
