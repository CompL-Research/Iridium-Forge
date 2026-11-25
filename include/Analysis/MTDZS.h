#pragma once

#include "Analysis/PassManager.h"
#include "Analysis/TDZA.h"
#include "Generated/IridiumTypes.h"
#include <iostream>

namespace IRI_STRUCTURAL {

// ============================================================================
// MTDZS (Mark TDZ Safe) Transformation Pass
// ============================================================================
struct MTDZSPass {
  bool run(IRICFG& cfg, AnalysisManager& am) {
    bool changed = false;
    BBContainerSupport bbc(cfg.container, cfg.ctx);

    // Query the TDZAnalysis result (this triggers TDZAnalysis if not cached)
    const TDZAnalysisResult& tdzResult = am.getResult<TDZAnalysis>(cfg);

    auto &ctx = cfg.ctx;

    TDZATransfer transfer;

    // Perform transformation logic using the analysis facts
    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      TDZState state = tdzResult.getBlockEntryState(h);

      // Edits queue in the editor, so the block's statement vector never moves
      // while this sweep runs and erasing the visited statement is safe.
      BBEditor ed(cfg, h);

      for (IRID stmtID : cfg.at(h).stmts) {
        auto stmtTag = IRI_NODE(ctx, stmtID).tag;
        bool removed = false;

        // Compute the next state before we potentially modify/delete stmtID
        TDZState nextState = transfer.transferStatement(ctx, stmtID, state);

        if (stmtTag == IRI_GEN::LWrite) {
          IRI_GEN::LWriteSEXP lw(stmtID, ctx);
          bool isInit = lw.hasINIT() || lw.hasTHISINIT();
          if (!isInit) {
            auto target = lw.getArg_LValTarget();
            bool willMarkSafe = !lw.hasSAFE() && !state.isUnreachable() && state.getLattice(target).kind == TDZLattice::SAFE;

            if (willMarkSafe) {
              lw.setSAFE();
              changed = true;
            }
          }
        } else if (stmtTag == IRI_GEN::EnvRead) {
          IRI_GEN::EnvReadSEXP envRead(stmtID, ctx);
          auto target = envRead.getArg_Obj();
          bool willRemove = !state.isUnreachable() && state.getLattice(target).kind == TDZLattice::SAFE;

          if (willRemove) {
            // Remove this instruction
            ed.erase(stmtID);
            removed = true;
          }
        }

        if (!removed) {
          auto processNestedEnvReads = [&](auto& self, IRID node) -> void {
            if (IRI_NODE(ctx, node).tag == IRI_GEN::EnvRead) {
              IRI_GEN::EnvReadSEXP envRead(node, ctx);
              auto target = envRead.getArg_Obj();
              bool nestedSafe = !state.isUnreachable() && state.getLattice(target).kind == TDZLattice::SAFE;

              if (nestedSafe && !envRead.hasSAFE()) {
                envRead.setSAFE();
                changed = true;
              }
            }
            for (auto child : ctx.storage.nodes.get_args_view(node)) {
              self(self, child);
            }
          };
          processNestedEnvReads(processNestedEnvReads, stmtID);
        }

        state = nextState;
      }

      changed |= ed.changed();
    }

    if (changed)
      am.invalidate(cfg);
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
