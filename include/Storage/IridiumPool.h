#pragma once
#include "Config.h"
#include "FlagInterning.h"
#include "Generated/IridiumEnums.h"
#include "IridiumSEXP.h"
#include "StringPool.h"
#include "external/Prakriti.hpp"
#include "external/pta_trace.hpp"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <span>
#include <unordered_map>
#include <vector>

namespace IRI_STRUCTURAL {
class IRIS;
class ClosureTree;
} // namespace IRI_STRUCTURAL

namespace IRI_STORAGE {
class IridiumPool {

public:
  IRID NULL_SEXP;
  IRID NOP_SEXP;
  IRID NUBD_SEXP;

  double lastBBIDX = 0;

  // Must be empty after stack->heap pass
  // added here as creating a new flag just for this is very memory
  // hungry
  std::unordered_map<IRID, double> CONTINUE_TARGETS;

  StringID getTemp();

  std::shared_ptr<IRI_STRUCTURAL::IRIS> iris = nullptr;
  std::shared_ptr<IRI_STRUCTURAL::ClosureTree> closureTree = nullptr;

  std::shared_ptr<ptf::TraceWriter> traceWriter = nullptr;
  std::function<std::unordered_map<std::string, std::string>(Prakriti::NodeUID)>
      traceNodeMetaMapper = nullptr;

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
