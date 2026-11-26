#pragma once

#include "Analysis/Liveness.h"
#include "Analysis/PassManager.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"

namespace IRI_STRUCTURAL {

// ============================================================================
// DCE (Dead Code Elimination) Transformation Pass
// ============================================================================
struct DCEPass {
  bool run(IRICFG &cfg, AnalysisManager &am) {
    auto &ctx = cfg.ctx;
    bool changed = false;
    const LivenessAnalysisResult &liveness =
        am.getResult<LivenessAnalysis>(cfg);

    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      // Dont DCE on the terminal, it can break IRICFG canonical form
      const std::vector<IRID> &stmts = cfg.at(h).stmts;
      BBEditor ed(cfg, h);

      for (size_t i = stmts.size(); i-- > 0;) {
        IRID stmtID = stmts[i];
        if (IRI_NODE(ctx, stmtID).tag != IRI_GEN::LWrite)
          continue;

        IRI_GEN::LWriteSEXP lw(stmtID, ctx);
        IRID target = lw.getArg_LValTarget();
        if (IRI_NODE(ctx, target).tag != IRI_GEN::EnvBinding)
          continue;

        if (liveness.getStateAfterStatement(h, i).getLattice(target).kind !=
            LivenessLattice::DEAD)
          continue;

        // Assigning to 'this' (initializing this after super) or a
        // non-initializing write to a const (this always throws an error, maybe
        // we should just replace the statement with an error)
        if (lw.hasTHISINIT() ||
            (IRI_GEN::EnvBindingSEXP(target, ctx).hasJSCONST() &&
             !lw.hasINIT()))
          continue;

        if (IRI_GEN::get_meta(IRI_NODE(ctx, lw.getArg_RVal()).tag) ==
            IRI_GEN::RVAL)
          ed.erase(stmtID);
        else
          ed.replace(stmtID, lw.getArg_RVal());
      }

      changed |= ed.changed();
    }

    if (changed)
      am.invalidate(cfg);
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
