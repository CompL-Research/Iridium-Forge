#pragma once

#include "Analysis/EffectAtStmt.h"
#include "Analysis/Liveness.h"
#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include <algorithm>
#include <set>
#include <vector>

namespace IRI_STRUCTURAL {

// ============================================================================
// Effect Propagation
//
// EffectAtStmt reports "binding x still holds exactly the value of expression
// e, computed at statement D". A read of x becomes e, and when x is dead
// afterwards D goes with it. Only worth doing when e is read once, or when
// duplicating e costs nothing.
// ============================================================================
struct EffectPropPass {
  // A literal can be duplicated as many times as it appears. Anything else is
  // only worth inlining once, as a move rather than a copy: JSArray, JSObject,
  // Lambda and RegExp all denote a freshly built value, so two copies of one of
  // them are two values.
  static bool isFreeToDuplicate(IRIContext &ctx, IRID node) {
    switch (IRI_NODE(ctx, node).tag) {
    case IRI_GEN::Number:
    case IRI_GEN::Boolean:
    case IRI_GEN::Null:
    case IRI_GEN::String:
    case IRI_GEN::JSBigInt:
    case IRI_GEN::JSNUBD:
      return true;
    default:
      return false;
    }
  }

  static size_t countReads(IRIContext &ctx, IRID node, IRID target) {
    size_t reads = IRI_NODE(ctx, node).tag == IRI_GEN::EnvRead &&
                           IRI_GEN::EnvReadSEXP(node, ctx).getArg_Obj() == target
                       ? 1
                       : 0;
    for (IRID child : ctx.storage.nodes.get_args_view(node))
      reads += countReads(ctx, child, target);
    return reads;
  }

  // Replaces every read of target in the arguments below node with effect, in
  // place. A read at the root cannot be replaced here, since that would mean
  // rewriting the statement itself, so this reports nothing done.
  static bool inlineReads(IRIContext &ctx, IRID node, IRID target,
                          IRID effect) {
    auto args = ctx.storage.nodes.get_args(node);
    bool inlined = false;
    for (size_t i = 0; i < args.size(); i++) {
      if (IRI_NODE(ctx, args[i]).tag == IRI_GEN::EnvRead &&
          IRI_GEN::EnvReadSEXP(args[i], ctx).getArg_Obj() == target) {
        ctx.storage.nodes.update_arg_inplace(node, i, effect);
        inlined = true;
      } else {
        inlined |= inlineReads(ctx, args[i], target, effect);
      }
    }
    return inlined;
  }

  bool run(IRICFG &cfg, AnalysisManager &am) {
    auto &ctx = cfg.ctx;
    bool changed = false;
    const EffectAtStmtResult &effects = am.getResult<EffectAtStmtAnalysis>(cfg);
    const LivenessAnalysisResult &liveness = am.getResult<LivenessAnalysis>(cfg);

    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      const std::vector<IRID> &stmts = cfg.at(h).stmts;
      std::set<IRID> definitions;

      for (size_t i = 0; i < stmts.size(); i++) {
        IRID stmtID = stmts[i];
        if (definitions.contains(stmtID))
          continue;

        // Re-queried every time rather than cached up front: the trees this
        // reads are the ones the earlier candidates in this sweep have already
        // rewritten.
        EffectAtStmtState effect = effects.getStateBeforeStatement(cfg, h, i);
        if (effect.kind != EffectAtStmtState::TRACKED)
          continue;

        IRID target = effect.store;
        bool deadAfter =
            liveness.getStateAfterStatement(h, i).getLattice(target).kind ==
            LivenessLattice::DEAD;

        size_t reads = countReads(ctx, stmtID, target);
        if (reads == 0)
          continue;
        if (!(reads == 1 && deadAfter) && !isFreeToDuplicate(ctx, effect.effect))
          continue;

        if (!inlineReads(ctx, stmtID, target, effect.effect))
          continue;
        changed = true;

        // The fact this write established reaches here without passing over any
        // other statement, so this read is the only one left after the
        // definition and the binding is dead from the definition onwards.
        if (deadAfter &&
            std::find(stmts.begin(), stmts.end(), effect.stmt) != stmts.end())
          definitions.insert(effect.stmt);
      }

      // Only definitions of this block: a definition elsewhere may still be
      // read on the way here, by a terminal condition for instance, which is
      // why liveness at this statement cannot see it.
      BBEditor ed(cfg, h);
      for (IRID def : definitions)
        ed.erase(def);
      changed |= ed.changed();
    }

    if (changed)
      am.invalidate(cfg);
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
