#pragma once

#include "Analysis/PassManager.h"
#include "Analysis/Liveness.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Support/BBContainerSupport.hpp"
#include <iostream>
#include <vector>

namespace IRI_STRUCTURAL {

// ============================================================================
// DCE (Dead Code Elimination) Transformation Pass
// ============================================================================
struct DCEPass {
  bool run(IRICFG& cfg, AnalysisManager& am) {
    bool changed = false;
    BBContainerSupport bbc(cfg.id, cfg.ctx);

    // Query the LivenessAnalysis result (triggers LivenessAnalysis if not cached)
    const LivenessAnalysisResult& livenessResult = am.getResult<LivenessAnalysis>(cfg);

    auto &ctx = cfg.ctx;
    LivenessTransfer transfer(bbc.hasSTRICT());

    // Perform transformation logic using the liveness facts
    for (const auto& [idx, bb] : cfg.nodeMap) {
      // Start with the exit state of the basic block (post-statement state)
      LivenessState state = livenessResult.getBlockExitState(idx);

      // Collect all statements in this basic block
      std::vector<IRIStatement*> stmts;
      for (IRIStatement* s = bb->head; s != nullptr; s = s->next) {
        stmts.push_back(s);
      }

      // Sound exceptional control flow propagation helper.
      // Since statements can throw and branch directly to the exception handler
      // block, the liveness facts of the catch block must merge back immediately
      // before executing any potentially throwing statement.
      auto handleException = [&](LivenessState& stateBefore) {
        if (bb->EXCEPTION > -1) {
          stateBefore = stateBefore.joinWith(livenessResult.getBlockEntryState(bb->EXCEPTION));
        }
      };

      // Process the terminal statement (bb->tail) first in backward order
      if (bb->tail) {
        IRIStatement* s = bb->tail;
        auto stmtID = s->id;
        auto stmtTag = IRI_NODE(ctx, stmtID).tag;

        // Compute the state immediately before the terminal statement
        LivenessState stateBefore = transfer.transferStatement(*s, state);
        handleException(stateBefore);

        // TODO: Check if the terminal statement is dead code (placeholder)

        state = stateBefore;
      }

      // Sweep non-terminal statements backward (from tail to head)
      for (auto it = stmts.rbegin(); it != stmts.rend(); ++it) {
        IRIStatement* s = *it;
        auto stmtID = s->id;
        auto stmtTag = IRI_NODE(ctx, stmtID).tag;

        // Compute the state immediately before this statement
        LivenessState stateBefore = transfer.transferStatement(*s, state);
        handleException(stateBefore);

        bool removed = false;
        if (stmtTag == IRI_GEN::LWrite) {
          IRI_GEN::LWriteSEXP lw(stmtID, ctx);
          auto target = lw.getArg_LValTarget();
          if (IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding) {
            EnvBindingSEXP ebb(target, ctx);
            auto kind = state.getLattice(target).kind;

            bool lvalIsDead = kind == LivenessLattice::DEAD;
            bool isThisInit = lw.hasTHISINIT();
            bool isConstWrite = ebb.hasJSCONST() && !lw.hasINIT();
            bool effectfulWrite = isThisInit || isConstWrite;
            if (lvalIsDead && !effectfulWrite) {
              auto rval = lw.getArg_RVal();
              bool safeToDelete = IRI_GEN::get_meta(IRI_NODE(ctx, rval).tag) == IRI_GEN::RVAL;
              if (safeToDelete) {
                bb->remove(s);
                removed = true;
                changed = true;
              } else {
                IRIStatement* newStmt = new IRIStatement(rval, bb.get());
                bb->replace(s, newStmt);
                changed = true;
                // Note: removed is left as false here so that the active liveness state
                // naturally falls through and updates to stateBefore, which correctly
                // contains the gen set of the reduced RHS expression.
              }
            }
          }
        }

        // Only advance the liveness state if the statement was not eliminated
        if (!removed) {
          state = stateBefore;
        }
      }
    }

    // Since a transformation pass modifies the IR/CFG, invalidate all cached analysis results
    am.invalidateAll();
    if (changed)
      cfg.markDirty();
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
