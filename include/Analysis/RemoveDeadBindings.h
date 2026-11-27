#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Support/IRIS.hpp"
#include <stdexcept>
#include <unordered_set>

namespace IRI_STRUCTURAL {

// ============================================================================
// Remove Dead Bindings Transformation Pass
// ============================================================================
struct RemoveDeadBindingsPass {
  bool run(IRICFG &cfg, AnalysisManager &am) {
    auto &ctx = cfg.ctx;

    std::unordered_set<IRID> referenced;
    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      const IRIBB &bb = cfg.at(h);
      if (bb.terminal != NO_IRID)
        collect(ctx, referenced, bb.terminal);
      for (IRID stmt : bb.stmts)
        collect(ctx, referenced, stmt);
    }

    SCPIDX headScope = cfg.at(cfg.entry).SCOPE;
    bool changed = false;
    for (IRID bID : ctx.iris->getEnvBindingsInClosure(headScope)) {
      if (referenced.contains(bID) || !deletable(ctx, bID))
        continue;
      IRI_GEN::EnvBindingSEXP eb(bID, ctx);
      ctx.iris->removeBinding(eb.getNAME(), eb.getSCOPE());
      changed = true;
    }

    if (changed)
      am.invalidate(cfg);
    return changed;
  }

private:
  static bool isBinding(IRI_TAG tag) {
    return tag == IRI_GEN::EnvBinding || tag == IRI_GEN::RemoteEnvBinding ||
           tag == IRI_GEN::ScriptBinding || tag == IRI_GEN::GlobalBinding;
  }

  void collect(IRIContext &ctx, std::unordered_set<IRID> &referenced,
               IRID node) {
    if (IRI_NODE(ctx, node).tag == IRI_GEN::UNOPDelVar &&
        !IRI_GEN::UNOPDelVarSEXP(node, ctx).hasGLOBAL())
      throw std::runtime_error(
          "[Forge]: UNOPDelVar reached the optimization passes without GLOBAL, "
          "its name refers to a binding nothing here can track");

    for (IRID child : ctx.storage.nodes.get_args_view(node)) {
      if (isBinding(IRI_NODE(ctx, child).tag))
        referenced.insert(child);
      collect(ctx, referenced, child);
    }
  }

  bool deletable(IRIContext &ctx, IRID bID) {
    if (IRI_NODE(ctx, bID).tag != IRI_GEN::EnvBinding)
      return false;
    auto &iris = ctx.iris;
    auto &meta = IRI_BINDING(iris, bID);
    if (meta.isCaptured() || meta.isEvalTainted() || meta.tombstone)
      return false;

    IRI_GEN::EnvBindingSEXP eb(bID, ctx);
    if (eb.hasJSARG() || eb.hasJSRESTARG())
      return false;
    return !iris->isExportedBinding(eb.getNAME());
  }
};

} // namespace IRI_STRUCTURAL
