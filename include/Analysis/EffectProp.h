#pragma once

#include "Analysis/PassManager.h"
#include "Analysis/EffectAtStmt.h"
#include "Analysis/Liveness.h"
#include "Generated/IridiumTypes.h"
#include "Support/BBContainerSupport.hpp"
#include <unordered_map>
#include <vector>
#include <set>

namespace IRI_STRUCTURAL {

struct EffectPropPass {
  bool run(IRICFG &cfg, AnalysisManager &am) {
    bool changed = false;

    // 1. Query the analysis results
    const EffectAtStmtResult &effectResult = am.getResult<EffectAtStmtAnalysis>(cfg);
    const LivenessAnalysisResult &livenessResult = am.getResult<LivenessAnalysis>(cfg);

    auto &ctx = cfg.ctx;

    BBContainerSupport bbc(cfg.id, ctx);
    bool strict = bbc.hasSTRICT();
    LivenessTransfer livenessTransfer(strict);

    // Check if a value node is safe to duplicate (constant or cheap read)
    auto isSafeToDup = [&](IRID node) -> bool {
      switch (IRI_NODE(ctx, node).tag) {
        case IRI_GEN::Number: case IRI_GEN::Boolean: case IRI_GEN::Null:
        case IRI_GEN::String: case IRI_GEN::JSBigInt: case IRI_GEN::JSNUBD:
          return true;
        case IRI_GEN::EnvRead: {
          IRI_GEN::EnvReadSEXP er(node, ctx);
          if (er.hasSAFE()) {
            IRID obj = er.getArg_Obj();
            if (IRI_NODE(ctx, obj).tag == IRI_GEN::GlobalBinding || IRI_NODE(ctx, obj).tag == IRI_GEN::ScriptBinding)
              return true;
          }
          break;
        }
        default: break;
      }
      return false;
    };

    // Perform optimization on each basic block
    for (const auto &[idx, bb] : cfg.nodeMap) {
      std::vector<IRIStatement*> stmts;
      for (IRIStatement *s = bb->head; s != nullptr; s = s->next) {
        stmts.push_back(s);
      }

      // Build ID -> statement map for O(1) defStmt lookup
      std::unordered_map<IRID, IRIStatement*> idToStmt;
      idToStmt.reserve(stmts.size());
      for (auto *s : stmts) idToStmt[s->id] = s;

      // Pre-compute per-statement liveness from a single backward pass
      // (eliminates O(n²) getLivenessAfterStatement re-traversals)
      std::vector<LivenessState> livenessAfter(stmts.size());
      {
        LivenessState state = livenessResult.getBlockExitState(bb->IDX);
        if (bb->tail && !(stmts.size() > 0 && stmts.back() == bb->tail))
          state = livenessTransfer.transferStatement(*bb->tail, state);
        for (ssize_t i = static_cast<ssize_t>(stmts.size()) - 1; i >= 0; --i) {
          livenessAfter[i] = state;
          state = livenessTransfer.transferStatement(**(stmts.begin() + static_cast<size_t>(i)), state);
        }
      }

      // Collect candidate indices upfront (skip if store liveness is TOP/unknown)
      std::vector<size_t> candidates;
      for (size_t i = 0; i < stmts.size(); ++i) {
        auto es = effectResult.queryStateAtStatement(*stmts[i]);
        if (!es.validEffect) continue;
        if (livenessAfter[i].getLattice(es.store).kind == LivenessLattice::TOP)
          continue; // unknown liveness → skip to avoid useless subtree walks
        candidates.push_back(i);
      }

      std::set<IRIStatement*> toRemove;

      for (size_t ci : candidates) {
        IRIStatement *s = stmts[ci];
        if (toRemove.count(s) > 0) continue;

        EffectAtStmtState state = effectResult.queryStateAtStatement(*s);
        IRID targetStore       = state.store;
        IRID replacementEffect = state.effect;
        IRID definitionStmtId  = state.stmt;

        // Liveness already cached — no re-traversal needed
        bool isDeadAfter = (livenessAfter[ci].getLattice(targetStore).kind != LivenessLattice::LIVE);

        // Cheap safety check before any subtree walk
        bool isSafeReadOrConstant = isSafeToDup(replacementEffect);

        // Count occurrences (only for candidates that passed the cheap checks)
        size_t occurrences = 0;
        {
          std::vector<IRID> worklist;
          worklist.push_back(s->id);
          while (!worklist.empty()) {
            IRID cur = worklist.back();
            worklist.pop_back();
            if (IRI_NODE(ctx, cur).tag == IRI_GEN::EnvRead) {
              IRI_GEN::EnvReadSEXP er(cur, ctx);
              if (er.getArg_Obj() == targetStore) ++occurrences;
            }
            for (auto child : ctx.storage.nodes.get_args_view(cur))
              worklist.push_back(child);
          }
        }
        if (occurrences == 0) continue;

        bool shouldInline = (occurrences == 1 && isDeadAfter) || isSafeReadOrConstant;
        if (!shouldInline) continue;

        // O(1) defStmt lookup via hash map instead of linear scan
        IRIStatement *defStmt = idToStmt.count(definitionStmtId) ? idToStmt[definitionStmtId] : nullptr;

        // Iterative tree walker to replace EnvRead(targetStore) with replacementEffect
        bool didPatch = false;
        {
          std::vector<std::pair<IRID, IRID>> path; // (parent, childToCheck)
          path.push_back({ 0, s->id });
          while (!path.empty()) {
            auto [parent, cur] = path.back();
            path.pop_back();
            if (IRI_NODE(ctx, cur).tag == IRI_GEN::EnvRead) {
              IRI_GEN::EnvReadSEXP er(cur, ctx);
              if (er.getArg_Obj() == targetStore) {
                // Replace in-place
                auto parentArgs = ctx.storage.nodes.get_args_view(parent);
                for (size_t ai = 0; ai < parentArgs.size(); ++ai) {
                  if (parentArgs[ai] == cur) {
                    ctx.storage.nodes.update_arg_inplace(parent, ai, replacementEffect);
                    didPatch = true;
                    break; // don't recurse into replaced node
                  }
                }
                continue;
              }
            }
            for (auto child : ctx.storage.nodes.get_args_view(cur))
              path.push_back({ cur, child });
          }
        }

        if (didPatch) {
          changed = true;
          if (isDeadAfter && defStmt) toRemove.insert(defStmt);
        }
      }

      // Remove the definitions of inlined effects
      for (auto *defStmt : toRemove) {
        bb->remove(defStmt);
      }
    }

    if (changed) {
      am.invalidateAll();
      cfg.markDirty();
    }
    return changed;
  }
};

} // namespace IRI_STRUCTURAL
