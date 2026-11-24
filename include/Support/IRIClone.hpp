#pragma once
#include "Generated/IridiumTypes.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/IRICFG.hpp"
#include <cstddef>
#include <unordered_map>
#include <vector>

namespace IRI_STRUCTURAL {

//
// Deep copy of pool nodes (duplicating BBs/subgraphs). Binding nodes are
// iris-owned singletons: clones share them by reference. targetMap remaps
// BBIDX-valued control flow targets and ExceptionIDX dispatch edges; a
// target outside the map (escaped) stays shared.
//
struct IRICloner {
  IRI_STORAGE::IRIContext &ctx;
  SCPIDX closureScope; // container scope keying ExceptionIDX entries
  std::unordered_map<IRI_STORAGE::IRID, IRI_STORAGE::IRID> nodeMap; // orig -> clone
  std::unordered_map<BBIDX, BBIDX> targetMap;                       // old BBIDX -> new BBIDX

  IRICloner(IRI_STORAGE::IRIContext &p, SCPIDX scope) : ctx(p), closureScope(scope) {}

  static bool isSingletonTag(IRI_GEN::IRI_TAG tag);
  static void remapTargets(
      IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID stmtID,
      const std::unordered_map<BBIDX, BBIDX> &targetMap);

  // Memoized deep copy of a node subtree.
  IRI_STORAGE::IRID cloneNode(IRI_STORAGE::IRID id);

  // cloneNode + control flow target remapping.
  IRI_STORAGE::IRID cloneStmt(IRI_STORAGE::IRID id);
};

// Copy a BB (flags/scope copied, IDX allocated by the caller); carries the
// source's ExceptionIDX entry, remapped through targetMap. Unattached.
IRI_GEN::IRID cloneBB(IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID bbID,
                      BBIDX newIDX, IRICloner &cloner);

// Move statements [fromIdx, end) of bbID into a new sibling BB; the suffix
// inherits the source's dispatch target under closureScope. Unattached.
IRI_GEN::IRID splitBBSuffix(IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID bbID,
                            size_t fromIdx, SCPIDX closureScope);

// Control-flow out-edges of a BB: jump targets and pushed catch entries.
std::vector<BBIDX> bbTargets(IRI_STORAGE::IRIContext &ctx,
                             IRI_STORAGE::IRID bbID);

// Result of cloneRegion.
struct RegionClone {
  IRICloner cloner; // targetMap: region source BBIDX -> clone BBIDX
  BBIDX entryIDX;   // the clone's entry block
  RegionClone(IRI_STORAGE::IRIContext &ctx, SCPIDX closureScope)
      : cloner(ctx, closureScope), entryIDX(NO_BB) {}
};

//
// Complete clone of the region rooted at entryIDX, attached to the
// container. Region = control-flow closure (bbTargets, so nested try/catch
// dispatch rides along) bounded to the entry's scope subtree; escape
// targets stay shared. Rewiring (terminals, PushCatchContext, ExceptionIDX)
// is internal; the source stays an unmodified template. InvokeFinalizer
// targets are not followed: nested finalizers must be expanded first.
//
RegionClone cloneRegion(IRI_STORAGE::IRIContext &ctx,
                        BBContainerSupport &bbc, BBIDX entryIDX);

} // namespace IRI_STRUCTURAL
