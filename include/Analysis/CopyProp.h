#pragma once

#include "Analysis/CopiesAtStmt.h"
#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"

namespace IRI_STRUCTURAL {

// ============================================================================
// Copy Propagation Transformation Pass
// ============================================================================
struct CopyPropPass {
  bool run(IRICFG &cfg, AnalysisManager &am) {
    bool changed = false;
    const CopiesAtStmtResult &copies = am.getResult<CopiesAtStmt>(cfg);

    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      const IRIBB &BB = cfg.at(h);

      for (size_t i = 0; i < BB.stmts.size(); i++)
        changed |= redirectReads(BB.stmts[i],
                                 copies.getStateBeforeStatement(h, i), cfg.ctx);

      if (BB.terminal != NO_IRID)
        changed |= redirectReads(
            BB.terminal, copies.getStateBeforeStatement(h, BB.stmts.size()),
            cfg.ctx);
    }

    if (changed)
      am.invalidate(cfg);
    return changed;
  }

private:
  bool redirectReads(IRID node, const CopyState &state, IRIContext &ctx) {
    bool changed = false;
    auto args = ctx.storage.nodes.get_args_view(node);
    for (size_t i = 0; i < args.size(); i++) {
      IRID child = args[i];
      if (IRI_NODE(ctx, child).tag == IRI_GEN::EnvRead) {
        IRI_GEN::EnvReadSEXP er(child, ctx);
        IRID target = er.getArg_Obj();
        if (IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding &&
            state.contains(target)) {
          CopyLattice fact = state.getLattice(target);
          if (fact.kind == CopyLattice::COPY) {
            if (IRI_NODE(ctx, node).tag == IRI_GEN::LWrite) {
              IRI_GEN::LWriteSEXP lw(node, ctx);
              if (lw.getArg_RVal() == child &&
                  lw.getArg_LValTarget() == fact.src)
                continue;
            }
            ctx.storage.nodes.update_arg_inplace(
                node, i,
                IRI_GEN::EnvReadSEXP::create(ctx, fact.src, true,
                                             er.hasTAINTED()));
            changed = true;
            continue;
          }
        }
      }
      changed |= redirectReads(child, state, ctx);
    }
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
