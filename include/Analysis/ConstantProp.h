#pragma once

#include "Analysis/PassManager.h"
#include "Analysis/ConstantsAtStmt.h"
#include "Generated/IridiumTypes.h"
#include "Storage/Config.h"
#include <vector>

namespace IRI_STRUCTURAL {

// ============================================================================
// Constant Propagation Transformation Pass
// ============================================================================
struct ConstantPropPass {
  bool run(IRICFG& cfg, AnalysisManager& am) {
    bool changed = false;
    const ConstantsAtStmtResult& casResult = am.getResult<ConstantsAtStmt>(cfg);

    for (BBIDX bbIDX = 0; bbIDX < cfg.numBBs(); bbIDX++) {
      const IRIBB & BB = cfg.at(bbIDX);
      const std::vector<IRID> & stmts = BB.stmts;

      for (size_t stmtOffset = 0; stmtOffset < stmts.size(); stmtOffset++) {
        IRID stmtID = stmts[stmtOffset];
        const CASState & stateBeforeStmt = casResult.getStateBeforeStatement(bbIDX, stmtOffset);
        changed |= propagateConstants(stmtID, stateBeforeStmt, cfg.ctx);
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
