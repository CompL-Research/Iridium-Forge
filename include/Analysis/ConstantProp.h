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
    BBContainerSupport bbc(cfg.id, cfg.pool);

    // Query the ConstantsAtStmt analysis result
    const ConstantsAtStmtResult& casResult = am.getResult<ConstantsAtStmt>(cfg);

    auto &pool = cfg.pool;

    ConstantsAtStmtTransfer transfer;

    // Perform transformation logic using the analysis facts
    for (const auto& [idx, bb] : cfg.nodeMap) {
      CASState state = casResult.getBlockEntryState(idx);
      IRIStatement* s = bb->head;
      while (s != nullptr) {
        IRIStatement* nextStmt = s->next;
        auto stmtID = s->id;
        auto stmtTag = pool[stmtID].tag;

        // Compute the next state before we potentially modify s
        CASState nextState = transfer.transferStatement(*s, state);

        // If the statement itself is a standalone EnvRead, replace it
        if (stmtTag == IRI_GEN::EnvRead) {
          // EnvReads at top level always unsafe, if they were safe they would have been collected by MTDZS pass..
          // Do nothing here
        } else {
          // Recursively find and replace EnvReads with constants in the statement's expression tree
          changed |= propagateConstants(stmtID, state, pool);
        }

        state = nextState;
        s = nextStmt;
      }
    }

    // Since a transformation pass modifies the IR/CFG, invalidate all cached analysis results
    am.invalidateAll();
    return changed;
  }

private:
  bool propagateConstants(IRID node, const CASState& state, IRI_STORAGE::IridiumPool& pool) {
    bool changed = false;
    auto args = pool.get_args_view(node);
    for (size_t i = 0; i < args.size(); ++i) {
      IRID child = args[i];
      auto childTag = pool[child].tag;
      if (childTag == IRI_GEN::EnvRead) {
        IRI_GEN::EnvReadSEXP envRead(child, pool);
        if (envRead.hasSAFE()) {
          auto target = envRead.getArg_Obj();
          if (pool[target].tag == IRI_GEN::EnvBinding) {
            auto lattice = state.getLattice(target);
            if (lattice.kind == CASLattice::CONST) {
              pool.update_arg_inplace(node, i, lattice.value);
              changed = true;
              continue;
            }
          }
        }
      }
      // Recurse
      changed |= propagateConstants(child, state, pool);
    }
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
