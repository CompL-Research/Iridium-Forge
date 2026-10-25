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
    BBContainerSupport bbc(cfg.id, cfg.ctx);

    // Query the ConstantsAtStmt analysis result
    const ConstantsAtStmtResult& casResult = am.getResult<ConstantsAtStmt>(cfg);

    auto &ctx = cfg.ctx;

    ConstantsAtStmtTransfer transfer;

    // Perform transformation logic using the analysis facts
    for (const auto& [idx, bb] : cfg.nodeMap) {
      CASState state = casResult.getBlockEntryState(idx);
      IRIStatement* s = bb->head;
      while (s != nullptr) {
        IRIStatement* nextStmt = s->next;
        auto stmtID = s->id;
        auto stmtTag = IRI_NODE(ctx, stmtID).tag;

        // Compute the next state before we potentially modify s
        CASState nextState = transfer.transferStatement(*s, state);

        // If the statement itself is a standalone EnvRead, replace it
        if (stmtTag == IRI_GEN::EnvRead) {
          // EnvReads at top level always unsafe, if they were safe they would have been collected by MTDZS pass..
          // Do nothing here
        } else {
          // Recursively find and replace EnvReads with constants in the statement's expression tree
          changed |= propagateConstants(stmtID, state, ctx);
        }

        state = nextState;
        s = nextStmt;
      }
    }

    // Since a transformation pass modifies the IR/CFG, invalidate all cached analysis results
    am.invalidateAll();
    if (changed)
      cfg.markDirty();
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
