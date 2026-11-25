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
    BBContainerSupport bbc(cfg.container, cfg.ctx);

    // Query the LivenessAnalysis result (triggers LivenessAnalysis if not cached)
    const LivenessAnalysisResult& livenessResult = am.getResult<LivenessAnalysis>(cfg);

    auto &ctx = cfg.ctx;
    LivenessTransfer transfer(bbc.hasSTRICT());

    // Perform transformation logic using the liveness facts
    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      const IRIBB &bb = cfg.at(h);

      // Start with the exit state of the basic block (post-statement state)
      LivenessState state = livenessResult.getBlockExitState(h);
      BBIDX handler = cfg.exceptionOf(h);

      // Sound exceptional control flow propagation helper.
      // Since statements can throw and branch directly to the exception handler
      // block, the liveness facts of the catch block must merge back immediately
      // before executing any potentially throwing statement.
      auto handleException = [&](LivenessState& stateBefore) {
        if (handler != NO_BB) {
          stateBefore = stateBefore.joinWith(livenessResult.getBlockEntryState(handler));
        }
      };

      // Process the terminal statement first in backward order
      if (bb.terminal != NO_IRID) {
        IRID stmtID = bb.terminal;

        // Compute the state immediately before the terminal statement
        LivenessState stateBefore = transfer.transferStatement(ctx, stmtID, state);
        handleException(stateBefore);

        // TODO: Check if the terminal statement is dead code (placeholder)

        state = stateBefore;
      }

      // Sweep non-terminal statements backward. The sweep reads the block's
      // statement list while the edits queue in the editor, so removing the
      // visited statement does not disturb the iteration.
      BBEditor ed(cfg, h);
      for (auto it = bb.stmts.rbegin(); it != bb.stmts.rend(); ++it) {
        IRID stmtID = *it;
        auto stmtTag = IRI_NODE(ctx, stmtID).tag;

        // Compute the state immediately before this statement
        LivenessState stateBefore = transfer.transferStatement(ctx, stmtID, state);
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
                ed.erase(stmtID);
                removed = true;
              } else {
                ed.replace(stmtID, rval);
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
      changed |= ed.changed();
    }

    if (changed)
      am.invalidate(cfg);
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
