#pragma once
#include "Config.h"
#include "FlagInterning.h"
#include "Generated/IridiumEnums.h"
#include "IridiumSEXP.h"
#include "StringPool.h"
#include <optional>
#include <set>
#include <span>
#include <unordered_map>
#include <vector>

namespace IRI_STORAGE {
class IridiumPool {

public:
  IRID NULL_SEXP;
  IRID NOP_SEXP;
  IRID UNDEF_SEXP;
  IRID NUBD_SEXP;

  std::optional<IRID> topLevelBBContainer;
  std::optional<double> topLevelScope;

  // IridiumPool() : NULL_SEXP() {}

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
};

} // namespace IRI_STORAGE
