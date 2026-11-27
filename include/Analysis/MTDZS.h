#pragma once

#include "Analysis/PassManager.h"
#include "Analysis/TDZA.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"

namespace IRI_STRUCTURAL {

// ============================================================================
// MTDZS (Mark TDZ Safe) Transformation Pass
// ============================================================================
struct MTDZSPass {
  static bool isSafe(const TDZState &state, IRID target) {
    return state.getLattice(target).kind == TDZLattice::SAFE;
  }

  static bool isEffectFree(IRIContext &ctx, IRID obj) {
    if (IRI_NODE(ctx, obj).tag != IRI_GEN::GlobalBinding)
      return false;
    IRI_GEN::GlobalBindingSEXP gb(obj, ctx);
    return ctx.storage.strings.get(gb.getNAME()) == "undefined";
  }

  static void markNested(IRIContext &ctx, IRID node, const TDZState &state,
                         bool &changed) {
    if (IRI_NODE(ctx, node).tag == IRI_GEN::EnvRead) {
      IRI_GEN::EnvReadSEXP er(node, ctx);
      IRID obj = er.getArg_Obj();
      if (!er.hasSAFE() && trackedBinding(ctx, obj) && isSafe(state, obj)) {
        er.setSAFE();
        changed = true;
      }
    }
    for (IRID child : ctx.storage.nodes.get_args_view(node))
      markNested(ctx, child, state, changed);
  }

  bool run(IRICFG &cfg, AnalysisManager &am) {
    auto &ctx = cfg.ctx;
    bool changed = false;
    const TDZAnalysisResult &tdz = am.getResult<TDZAnalysis>(cfg);

    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      const std::vector<IRID> &stmts = cfg.at(h).stmts;
      BBEditor ed(cfg, h);

      for (size_t i = 0; i < stmts.size(); i++) {
        IRID stmtID = stmts[i];
        IRI_TAG tag = IRI_NODE(ctx, stmtID).tag;
        const TDZState &state = tdz.getStateBeforeStatement(h, i);

        if (tag == IRI_GEN::EnvRead) {
          IRID obj = IRI_GEN::EnvReadSEXP(stmtID, ctx).getArg_Obj();
          if (isEffectFree(ctx, obj) ||
              (trackedBinding(ctx, obj) && isSafe(state, obj))) {
            ed.erase(stmtID);
            continue;
          }
        }

        markNested(ctx, stmtID, state, changed);
      }

      IRID terminal = cfg.at(h).terminal;
      if (terminal != NO_IRID)
        markNested(ctx, terminal,
                   tdz.getStateBeforeStatement(h, stmts.size()), changed);

      changed |= ed.changed();
    }

    if (changed)
      am.invalidate(cfg);
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
