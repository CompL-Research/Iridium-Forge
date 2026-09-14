#pragma once

#include "Support/AbstractInterpretation.hpp"
#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/IRIS.hpp"
#include "Support/BBContainerSupport.hpp"
#include <memory>
#include <set>

namespace IRI_STRUCTURAL {

// ============================================================================
// 1. EffectAtStmt Lattice Definition (State)
// ============================================================================
class EffectAtStmtState {
private:
  bool is_unreachable = true;

public:
  bool validEffect = false;
  IRID store = 0;
  IRID effect = 0;
  IRID stmt = 0;

  EffectAtStmtState() : is_unreachable(true), validEffect(false), store(0), effect(0), stmt(0) {}
  EffectAtStmtState(bool unreachable, bool valid, IRID s, IRID e, IRID st)
      : is_unreachable(unreachable), validEffect(valid), store(s), effect(e), stmt(st) {}

  static EffectAtStmtState bottom() {
    return EffectAtStmtState();
  }

  static EffectAtStmtState reachableEmpty() {
    return EffectAtStmtState(false, false, 0, 0, 0);
  }

  bool isUnreachable() const { return is_unreachable; }

  void dump(IRI_STORAGE::IRIContext &ctx, std::ostream &oss) const {
    if (is_unreachable) {
      oss << "<Unreachable>";
      return;
    }
    oss << "{";
    if (validEffect) {
      if (IRI_NODE(ctx, store).tag == IRI_GEN::EnvBinding) {
        IRI_GEN::EnvBindingSEXP eb(store, ctx);
        oss << ctx.storage.strings.get(eb.getNAME()) << " -> IRID(" << effect << ")";
      } else {
        oss << "IRID(" << store << ") -> IRID(" << effect << ")";
      }
    } else {
      oss << "INVALID";
    }
    oss << "}";
  }

  [[nodiscard]] EffectAtStmtState joinWith(const EffectAtStmtState &other) const {
    if (is_unreachable)
      return other;
    if (other.is_unreachable)
      return *this;

    if (validEffect == other.validEffect && store == other.store && effect == other.effect && stmt == other.stmt) {
      return *this;
    }
    return EffectAtStmtState(false, false, 0, 0, 0);
  }

  bool operator==(const EffectAtStmtState &other) const {
    if (is_unreachable != other.is_unreachable)
      return false;
    if (is_unreachable)
      return true;
    return validEffect == other.validEffect && store == other.store && effect == other.effect && stmt == other.stmt;
  }
};

// ============================================================================
// 2. EffectAtStmt Transfer Function
// ============================================================================
class EffectAtStmtTransfer : public TransferFunction<EffectAtStmtState> {
public:
  static bool isSideEffectingTag(IRI_GEN::IRI_TAG tag, IRID node, IRI_STORAGE::IRIContext &ctx) {
    auto meta = IRI_GEN::get_meta(tag);
    if (meta == IRI_GEN::RVAL || meta == IRI_GEN::UKN) {
      return false;
    }
    // Special case: EnvRead is side-effect-free ONLY if it is marked SAFE and is a local non-captured variable
    if (tag == IRI_GEN::EnvRead) {
      IRI_GEN::EnvReadSEXP er(node, ctx);
      if (er.hasSAFE()) {
        auto obj = er.getArg_Obj();
        if (IRI_NODE(ctx, obj).tag == IRI_GEN::EnvBinding && !IRI_BINDING(ctx.iris, obj).isCaptured()) {
          return false;
        }
      }
    }
    return true;
  }

  EffectAtStmtState transferStatement(const IRIStatement &stmt,
                                      const EffectAtStmtState &incomingState) override {
    if (incomingState.isUnreachable()) {
      return EffectAtStmtState::bottom();
    }

    EffectAtStmtState nextState = incomingState;
    auto &ctx = stmt.bb->ctx;
    auto tag = IRI_NODE(ctx, stmt.id).tag;

    // Recursive helper to check for side-effecting sub-expressions
    auto hasSideEffect = [&](auto &self, IRID node) -> bool {
      auto t = IRI_NODE(ctx, node).tag;
      if (isSideEffectingTag(t, node, ctx)) {
        return true;
      }
      for (auto child : ctx.storage.nodes.get_args_view(node)) {
        if (self(self, child)) {
          return true;
        }
      }
      return false;
    };

    // Recursive helper to check for reads/writes to captured variables
    auto hasCapturedAccess = [&](auto &self, IRID node) -> bool {
      auto t = IRI_NODE(ctx, node).tag;
      if (t == IRI_GEN::EnvRead) {
        IRI_GEN::EnvReadSEXP er(node, ctx);
        auto target = er.getArg_Obj();
        if (IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding) {
          if (IRI_BINDING(ctx.iris, target).isCaptured()) {
            return true;
          }
        }
      } else if (t == IRI_GEN::LWrite) {
        IRI_GEN::LWriteSEXP lw(node, ctx);
        auto target = lw.getArg_LValTarget();
        if (IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding) {
          if (IRI_BINDING(ctx.iris, target).isCaptured()) {
            return true;
          }
        }
      }
      for (auto child : ctx.storage.nodes.get_args_view(node)) {
        if (self(self, child)) {
          return true;
        }
      }
      return false;
    };

    // Recursive helper to check if a node contains a specific target node
    auto containsNode = [&](auto &self, IRID node, IRID target) -> bool {
      if (node == target) {
        return true;
      }
      for (auto child : ctx.storage.nodes.get_args_view(node)) {
        if (self(self, child, target)) {
          return true;
        }
      }
      return false;
    };

    // Recursive helper to collect all bindings read in an expression
    auto getReadBindings = [&](auto &self, IRID node, std::set<IRID> &bindings) -> void {
      auto t = IRI_NODE(ctx, node).tag;
      if (t == IRI_GEN::EnvRead) {
        IRI_GEN::EnvReadSEXP er(node, ctx);
        auto target = er.getArg_Obj();
        if (IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding) {
          bindings.insert(target);
        }
      }
      for (auto child : ctx.storage.nodes.get_args_view(node)) {
        self(self, child, bindings);
      }
    };

    if (tag == IRI_GEN::LWrite) {
      IRI_GEN::LWriteSEXP lw(stmt.id, ctx);
      auto LVAL = lw.getArg_LValTarget();
      auto rval = lw.getArg_RVal();

      if (nextState.validEffect && nextState.store != LVAL) {
        // Check for invalidation of the tracked effect
        bool kill = false;

        // 1. AnyWritesToBindingsUsedInEffect
        std::set<IRID> bindingsUsed;
        getReadBindings(getReadBindings, nextState.effect, bindingsUsed);
        if (bindingsUsed.count(LVAL) > 0) {
          kill = true;
        }

        // 2. AnyReadsToStore
        if (containsNode(containsNode, rval, nextState.store)) {
          kill = true;
        }

        // 3. AnyReadsOrWritesToCapturedVariables
        if (hasCapturedAccess(hasCapturedAccess, stmt.id)) {
          kill = true;
        }

        // 4. RVAL maybeEffectful
        if (hasSideEffect(hasSideEffect, rval)) {
          kill = true;
        }

        if (kill) {
          nextState = EffectAtStmtState::reachableEmpty();
        }
      }

      // Overwrite/Update state after the kill checks so we don't kill ourselves immediately.
      // Only track effect-free writes: if RVal has side effects, don't mark as valid.
      if (IRI_NODE(ctx, LVAL).tag == IRI_GEN::EnvBinding && !IRI_BINDING(ctx.iris, LVAL).isCaptured() &&
          !hasSideEffect(hasSideEffect, rval)) {
        nextState = EffectAtStmtState(false, true, LVAL, rval, stmt.id);
      } else {
        // Any write to a global, remote, or captured variable kills the currently tracked effect
        nextState = EffectAtStmtState::reachableEmpty();
      }
    } else {
      // Non-LWrite statement
      if (nextState.validEffect) {
        bool kill = false;
        if (containsNode(containsNode, stmt.id, nextState.store)) {
          kill = true;
        }
        if (hasSideEffect(hasSideEffect, stmt.id)) {
          kill = true;
        }
        if (hasCapturedAccess(hasCapturedAccess, stmt.id)) {
          kill = true;
        }
        if (kill) {
          nextState = EffectAtStmtState::reachableEmpty();
        }
      }
    }

    return nextState;
  }

  EffectAtStmtState transferEdge(BBIDX from, BBIDX to,
                                 const EffectAtStmtState &exitState) override {
    return exitState;
  }
};

// ============================================================================
// 3. EffectAtStmt Analysis Result
// ============================================================================
class EffectAtStmtResult : public DataflowResultConcept {
private:
  std::shared_ptr<EffectAtStmtTransfer> transfer;
  std::shared_ptr<DataflowSolver<EffectAtStmtState>> solver;

public:
  EffectAtStmtResult() = default;
  EffectAtStmtResult(std::shared_ptr<EffectAtStmtTransfer> t,
                     std::shared_ptr<DataflowSolver<EffectAtStmtState>> s)
      : transfer(std::move(t)), solver(std::move(s)) {}

  std::string getAnalysisName() const override { return "EffectAtStmt"; }

  void dumpStateAtStatement(const IRIStatement &stmt,
                            IRI_STORAGE::IRIContext &ctx,
                            std::ostream &os) const override {
    EffectAtStmtState state = queryStateAtStatement(stmt);
    state.dump(ctx, os);
  }

  void dumpBlockEntryState(BBIDX block, IRI_STORAGE::IRIContext &ctx,
                           std::ostream &os) const override {
    const EffectAtStmtState &state = getBlockEntryState(block);
    state.dump(ctx, os);
  }

  void dumpBlockExitState(BBIDX block, IRI_STORAGE::IRIContext &ctx,
                          std::ostream &os) const override {
    os << "<ExitStateNotTracked>";
  }

  const EffectAtStmtState &getBlockEntryState(BBIDX block) const {
    return solver->getBlockEntryState(block);
  }

  EffectAtStmtState queryStateAtStatement(const IRIStatement &stmt) const {
    return solver->queryStateAtStatement(stmt);
  }
};

// ============================================================================
// 4. EffectAtStmt Analysis Definition
// ============================================================================
struct EffectAtStmtAnalysis {
  static inline char ID = 0;
  using Result = EffectAtStmtResult;

  EffectAtStmtResult run(IRICFG &cfg, AnalysisManager &am) {
    auto transfer = std::make_shared<EffectAtStmtTransfer>();
    auto solver = std::make_shared<DataflowSolver<EffectAtStmtState>>(cfg, *transfer);

    EffectAtStmtState entryState = EffectAtStmtState::reachableEmpty();

    solver->run(entryState, /*includeExceptions=*/true);

    return EffectAtStmtResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
