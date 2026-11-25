#pragma once

#include "Analysis/PassManager.h"
#include "Analysis/ConstantsAtStmt.h"
#include "Generated/IridiumTypes.h"
#include "Support/BBContainerSupport.hpp"
#include <iostream>
#include <vector>

namespace IRI_STRUCTURAL {

// ============================================================================
// Constant Propagation Transformation Pass
// ============================================================================
struct ConstantPropPass {
  bool run(IRICFG& cfg, AnalysisManager& am) {
    bool changed = false;
    BBContainerSupport bbc(cfg.container, cfg.ctx);

    // 1. Get ConstantsAtStmt
    const ConstantsAtStmtResult& casResult = am.getResult<ConstantsAtStmt>(cfg);

    ConstantsAtStmtTransfer transfer;
    // Perform transformation logic using the analysis facts
    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      CASState state = casResult.getBlockEntryState(h);
      // Only expression trees are rewritten, never the statement list, so no
      // BBEditor is needed.
      for (IRID stmtID : cfg.at(h).stmts) {
        auto stmtTag = IRI_NODE(cfg.ctx, stmtID).tag;

        // Compute the next state before we potentially modify stmtID
        CASState nextState = transfer.transferStatement(cfg.ctx, stmtID, state);

        // If the statement itself is a standalone EnvRead, replace it
        if (stmtTag == IRI_GEN::EnvRead) {
          // EnvReads at top level always unsafe, if they were safe they would have been collected by MTDZS pass..
          // Do nothing here
        } else {
          // Recursively find and replace EnvReads with constants in the statement's expression tree
          changed |= propagateConstants(stmtID, state, cfg.ctx);
        }

        state = nextState;
      }
    }

    if (changed)
      am.invalidate(cfg);
    return changed;
  }

private:
  bool propagateConstants(IRID node, const CASState& state, IRI_STORAGE::IRIContext& ctx) {
    bool changed = false;
    auto args = ctx.storage.nodes.get_args_view(node);
    for (size_t i = 0; i < args.size(); ++i) {
      IRID child = args[i];
      auto childTag = IRI_NODE(ctx, child).tag;
      if (childTag == IRI_GEN::EnvRead) {
        IRI_GEN::EnvReadSEXP envRead(child, ctx);
        if (envRead.hasSAFE()) {
          auto target = envRead.getArg_Obj();
          if (IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding) {
            auto lattice = state.getLattice(target);
            if (lattice.kind == CASLattice::CONST) {
              ctx.storage.nodes.update_arg_inplace(node, i, lattice.value);
              changed = true;
              continue;
            }
          }
        }
      }
      // Recurse
      changed |= propagateConstants(child, state, ctx);
    }
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
