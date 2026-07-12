#pragma once

#include "Analysis/PassManager.h"
#include "Analysis/EffectAtStmt.h"
#include "Analysis/Liveness.h"
#include "Generated/IridiumTypes.h"
#include "Support/BBContainerSupport.hpp"
#include <iostream>
#include <vector>
#include <set>

namespace IRI_STRUCTURAL {

struct EffectPropPass {
  bool run(IRICFG &cfg, AnalysisManager &am) {
    bool changed = false;
    
    // 1. Query the analysis results
    const EffectAtStmtResult &effectResult = am.getResult<EffectAtStmtAnalysis>(cfg);
    const LivenessAnalysisResult &livenessResult = am.getResult<LivenessAnalysis>(cfg);

    auto &pool = cfg.pool;
    
    // Get strict mode flag
    BBContainerSupport bbc(cfg.id, pool);
    bool strict = bbc.hasSTRICT();
    LivenessTransfer livenessTransfer(strict);

    // Helper to count occurrences of targetStore in expression node
    auto countOccurrences = [&](auto &self, IRID node, IRID targetStore) -> size_t {
      size_t count = 0;
      auto tag = pool[node].tag;
      if (tag == IRI_GEN::EnvRead) {
        IRI_GEN::EnvReadSEXP er(node, pool);
        if (er.getArg_Obj() == targetStore) {
          count++;
        }
      }
      for (auto child : pool.get_args_view(node)) {
        count += self(self, child, targetStore);
      }
      return count;
    };

    // Helper to replace targetStore EnvRead with replacementEffect
    auto patchNode = [&](auto &self, IRID node, IRID targetStore, IRID replacementEffect) -> bool {
      bool replaced = false;
      auto args = pool.get_args_view(node);
      for (size_t i = 0; i < args.size(); ++i) {
        IRID child = args[i];
        auto tag = pool[child].tag;
        if (tag == IRI_GEN::EnvRead) {
          IRI_GEN::EnvReadSEXP er(child, pool);
          if (er.getArg_Obj() == targetStore) {
            pool.update_arg_inplace(node, i, replacementEffect);
            replaced = true;
            continue;
          }
        }
        if (self(self, child, targetStore, replacementEffect)) {
          replaced = true;
        }
      }
      return replaced;
    };

    // Helper to calculate liveness state after statement stmt
    auto getLivenessAfterStatement = [&](const IRIStatement &stmt) -> LivenessState {
      LivenessState state = livenessResult.getBlockExitState(stmt.bb->IDX);
      if (stmt.bb->tail && stmt.bb->tail != &stmt) {
        state = livenessTransfer.transferStatement(*stmt.bb->tail, state);
      }
      std::vector<IRIStatement *> stmts;
      for (IRIStatement *s = stmt.bb->head; s != nullptr; s = s->next) {
        stmts.push_back(s);
      }
      for (auto it = stmts.rbegin(); it != stmts.rend(); ++it) {
        if (*it == &stmt) {
          return state;
        }
        state = livenessTransfer.transferStatement(**it, state);
      }
      return state;
    };

    // Perform optimization on each basic block
    for (const auto &[idx, bb] : cfg.nodeMap) {
      std::vector<IRIStatement*> stmts;
      for (IRIStatement *s = bb->head; s != nullptr; s = s->next) {
        stmts.push_back(s);
      }

      std::set<IRIStatement*> toRemove;

      for (auto *s : stmts) {
        // Skip statements already scheduled for removal
        if (toRemove.count(s) > 0) continue;

        EffectAtStmtState state = effectResult.queryStateAtStatement(*s);
        if (state.validEffect) {
          IRID targetStore = state.store;
          IRID replacementEffect = state.effect;
          IRID definitionStmtId = state.stmt;

          // Check if the replacement effect is safe to duplicate
          bool isSafeReadOrConstant = false;
          auto repTag = pool[replacementEffect].tag;
          if (repTag == IRI_GEN::Number || repTag == IRI_GEN::Boolean || repTag == IRI_GEN::Null ||
              repTag == IRI_GEN::String || repTag == IRI_GEN::JSBigInt || repTag == IRI_GEN::JSNUBD) {
            isSafeReadOrConstant = true;
          } else if (repTag == IRI_GEN::EnvRead) {
            IRI_GEN::EnvReadSEXP er(replacementEffect, pool);
            if (er.hasSAFE()) {
              isSafeReadOrConstant = true;
            } else {
              auto obj = er.getArg_Obj();
              auto objTag = pool[obj].tag;
              if (objTag == IRI_GEN::GlobalBinding || objTag == IRI_GEN::ScriptBinding) {
                isSafeReadOrConstant = true;
              }
            }
          }

          LivenessState livenessAfter = getLivenessAfterStatement(*s);
          bool isDeadAfter = (livenessAfter.getLattice(targetStore).kind != LivenessLattice::LIVE);

          size_t occurrences = countOccurrences(countOccurrences, s->id, targetStore);
          
          bool shouldInline = false;
          if (occurrences == 1 && isDeadAfter) {
            shouldInline = true;
          } else if (occurrences > 0 && isSafeReadOrConstant) {
            shouldInline = true;
          }

          if (shouldInline) {
            // Find definition statement
            IRIStatement* defStmt = nullptr;
            for (auto *stmtInBB : stmts) {
              if (stmtInBB->id == definitionStmtId) {
                defStmt = stmtInBB;
                break;
              }
            }

            if (patchNode(patchNode, s->id, targetStore, replacementEffect)) {
              if (isDeadAfter && defStmt) {
                toRemove.insert(defStmt);
              }
              changed = true;
            }
          }
        }
      }

      // Remove the definitions of inlined effects
      for (auto *defStmt : toRemove) {
        bb->remove(defStmt);
      }
    }

    if (changed) {
      am.invalidateAll();
    }
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
