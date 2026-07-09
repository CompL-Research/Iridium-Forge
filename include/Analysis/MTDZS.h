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
  void run(IRICFG& cfg, AnalysisManager& am) {
    BBContainerSupport bbc(cfg.id, cfg.pool);
    std::cout << "  Running MTDZSPass on CFG of closure " << bbc.getStartBBIDX() << ":\n";

    // Query the TDZAnalysis result (this triggers TDZAnalysis if not cached)
    const TDZAnalysisResult& tdzResult = am.getResult<TDZAnalysis>(cfg);

    auto &pool = cfg.pool;

    TDZATransfer transfer;

    // Perform transformation logic using the analysis facts
    for (const auto& [idx, bb] : cfg.nodeMap) {
      TDZState state = tdzResult.getBlockEntryState(idx);
      IRIStatement* s = bb->head;
      while (s != nullptr) {
        IRIStatement* nextStmt = s->next;
        auto stmtID = s->id;
        auto stmtTag = pool[stmtID].tag;
        bool removed = false;

        // Compute the next state before we potentially modify/delete s
        TDZState nextState = transfer.transferStatement(*s, state);

        if (stmtTag == IRI_GEN::LWrite) {
          IRI_GEN::LWriteSEXP lw(stmtID, pool);
          bool isInit = lw.hasINIT() || lw.hasTHISINIT();
          if (!isInit) {
            auto target = lw.getArg_LValTarget();
            bool willMarkSafe = !lw.hasSAFE() && !state.isUnreachable() && state.getLattice(target).kind == TDZLattice::SAFE;
#if DEBUG_TDZ
            std::cout << "    [MTDZS] LWrite stmt ID: " << stmtID << " target: ";
            if (pool[target].tag == IRI_GEN::EnvBinding) {
              IRI_GEN::EnvBindingSEXP eb(target, pool);
              std::cout << pool.strings.get(eb.getNAME());
            } else {
              std::cout << "IRID(" << target << ")";
            }
            std::cout << " State: ";
            state.dump(pool, std::cout);
            std::cout << " -> " << (willMarkSafe ? "MARKING SAFE" : "KEEPING TDZ") << "\n";
#endif
            if (willMarkSafe) {
              lw.setSAFE();
            }
          }
        } else if (stmtTag == IRI_GEN::EnvRead) {
          IRI_GEN::EnvReadSEXP envRead(stmtID, pool);
          auto target = envRead.getArg_Obj();
          bool willRemove = !state.isUnreachable() && state.getLattice(target).kind == TDZLattice::SAFE;
#if DEBUG_TDZ
          std::cout << "    [MTDZS] EnvRead stmt ID: " << stmtID << " target: ";
          if (pool[target].tag == IRI_GEN::EnvBinding) {
            IRI_GEN::EnvBindingSEXP eb(target, pool);
            std::cout << pool.strings.get(eb.getNAME());
          } else {
            std::cout << "IRID(" << target << ")";
          }
          std::cout << " State: ";
          state.dump(pool, std::cout);
          std::cout << " -> " << (willRemove ? "REMOVING" : "KEEPING") << "\n";
#endif
          if (willRemove) {
            // Remove this instruction
            bb->remove(s);
            removed = true;
          }
        }

        if (!removed) {
          auto processNestedEnvReads = [&](auto& self, IRID node) -> void {
            if (pool[node].tag == IRI_GEN::EnvRead) {
              IRI_GEN::EnvReadSEXP envRead(node, pool);
              auto target = envRead.getArg_Obj();
              bool nestedSafe = !state.isUnreachable() && state.getLattice(target).kind == TDZLattice::SAFE;
#if DEBUG_TDZ
              std::cout << "      [MTDZS-Nested] EnvRead ID: " << node << " target: ";
              if (pool[target].tag == IRI_GEN::EnvBinding) {
                IRI_GEN::EnvBindingSEXP eb(target, pool);
                std::cout << pool.strings.get(eb.getNAME());
              } else {
                std::cout << "IRID(" << target << ")";
              }
              std::cout << " State: ";
              state.dump(pool, std::cout);
              std::cout << " -> " << (nestedSafe ? "MARKING SAFE" : "KEEPING TDZ") << "\n";
#endif
              if (nestedSafe) {
                envRead.setSAFE();
              }
            }
            for (auto child : pool.get_args_view(node)) {
              self(self, child);
            }
          };
          processNestedEnvReads(processNestedEnvReads, stmtID);
        }

        state = nextState;
        s = nextStmt;
      }
    }



    // Since a transformation pass modifies the IR/CFG, invalidate all cached analysis results
    am.invalidateAll();
  }
};

} // namespace IRI_STRUCTURAL
