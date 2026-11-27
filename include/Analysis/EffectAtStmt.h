#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/IRIS.hpp"
#include <memory>
#include <set>

namespace IRI_STRUCTURAL {

//
// State
//
// The fact carried is "this binding still holds exactly the value this
// expression computed", so a later read of the binding can be replaced by that
// expression. It is a must fact: it survives only while every path agrees.
//
//   UNKNOWN   no predecessor has reported yet, the join identity
//   NOEFFECT  no binding's value is usable here, absorbing
//   TRACKED   store holds the value effect computes, as of stmt
//
// One binding is tracked at a time. Every statement either replaces the tracked
// value or destroys it, so a fact only ever reaches the statement right after
// the write that created it and a chain of n copies takes n sweeps to travel.
// That is what makes this analysis cheap to compute and slow to converge.
//
struct EffectAtStmtState {
  enum Kind { UNKNOWN, NOEFFECT, TRACKED };
  Kind kind = UNKNOWN;
  IRID store = NO_IRID;
  IRID effect = NO_IRID;
  IRID stmt = NO_IRID;

  EffectAtStmtState() = default;
  EffectAtStmtState(Kind k) : kind(k) {}
  EffectAtStmtState(IRID store, IRID effect, IRID stmt)
      : kind(TRACKED), store(store), effect(effect), stmt(stmt) {}

  [[nodiscard]] EffectAtStmtState
  joinWith(const EffectAtStmtState &other) const {
    if (kind == UNKNOWN)
      return other;
    if (other.kind == UNKNOWN)
      return *this;
    if (*this == other)
      return *this;
    return EffectAtStmtState(NOEFFECT);
  }

  bool operator==(const EffectAtStmtState &other) const {
    if (kind != other.kind)
      return false;
    if (kind != TRACKED)
      return true;
    return store == other.store && effect == other.effect &&
           stmt == other.stmt;
  }
};

//
// Transfer Function
//
class EffectAtStmtTransfer : public TransferFunction<EffectAtStmtState> {
public:
  EffectAtStmtState
  transferStatement(IRIContext &ctx, IRID stmt,
                    const EffectAtStmtState &incoming) override {
    // Only a local write can establish a value, and no other statement can be
    // shown to leave the current one intact. Being strict here is what makes
    // eval, delete x and calls safe without reasoning about them: all three are
    // AMP nodes, so they are side effects and they end the fact.
    if (IRI_NODE(ctx, stmt).tag != IRI_GEN::LWrite)
      return EffectAtStmtState(EffectAtStmtState::NOEFFECT);

    IRI_GEN::LWriteSEXP lw(stmt, ctx);
    IRID target = lw.getArg_LValTarget();
    IRID rval = lw.getArg_RVal();

    // Nothing new is established. The value tracked before the write survives
    // only if this write cannot reach it.
    auto lost = [&] { return survives(ctx, incoming, target, rval); };

    if (IRI_NODE(ctx, target).tag != IRI_GEN::EnvBinding)
      return lost();
    // A captured binding can be written by a nested closure before the read.
    if (IRI_BINDING(ctx.iris, target).isCaptured())
      return lost();
    if (hasSideEffect(ctx, rval))
      return lost();
    // The value must not read the binding it is stored to. At a later read,
    // that read denotes the new value, not the one the tree was built from, so
    // x = [x] would inline to a tree reading itself.
    if (contains(ctx, rval, target))
      return lost();

    return EffectAtStmtState(target, rval, stmt);
  }

private:
  // The value tracked before a write survives it unless the write stores to the
  // tracked binding, stores to a binding the tracked expression reads, or runs
  // something that could change that expression's value.
  static EffectAtStmtState survives(IRIContext &ctx,
                                    const EffectAtStmtState &state, IRID target,
                                    IRID rval) {
    if (state.kind == EffectAtStmtState::NOEFFECT)
      return state;
    if (state.kind == EffectAtStmtState::TRACKED && state.store != target &&
        !readsBinding(ctx, state.effect, target) && !hasSideEffect(ctx, rval))
      return state;
    return EffectAtStmtState(EffectAtStmtState::NOEFFECT);
  }

  // A node that is not a pure value. A checked read of a local, uncaptured
  // binding is just a stack slot, so it is the one read that is pure; anything
  // else, including a read of a global or a captured binding, can observe or be
  // changed by the rest of the program.
  static bool isSideEffecting(IRI_GEN::IRI_TAG tag, IRID node,
                              IRIContext &ctx) {
    auto meta = IRI_GEN::get_meta(tag);
    if (meta == IRI_GEN::RVAL || meta == IRI_GEN::UKN)
      return false;
    if (tag == IRI_GEN::EnvRead) {
      IRI_GEN::EnvReadSEXP er(node, ctx);
      IRID obj = er.getArg_Obj();
      if (er.hasSAFE() && IRI_NODE(ctx, obj).tag == IRI_GEN::EnvBinding &&
          !IRI_BINDING(ctx.iris, obj).isCaptured())
        return false;
    }
    return true;
  }

  static bool hasSideEffect(IRIContext &ctx, IRID node) {
    if (isSideEffecting(IRI_NODE(ctx, node).tag, node, ctx))
      return true;
    for (IRID child : ctx.storage.nodes.get_args_view(node))
      if (hasSideEffect(ctx, child))
        return true;
    return false;
  }

  static bool contains(IRIContext &ctx, IRID node, IRID target) {
    if (node == target)
      return true;
    for (IRID child : ctx.storage.nodes.get_args_view(node))
      if (contains(ctx, child, target))
        return true;
    return false;
  }

  // Does this value read the given binding, which is what decides whether a
  // write to it changes the value.
  static bool readsBinding(IRIContext &ctx, IRID node, IRID target) {
    if (IRI_NODE(ctx, node).tag == IRI_GEN::EnvRead &&
        IRI_GEN::EnvReadSEXP(node, ctx).getArg_Obj() == target)
      return true;
    for (IRID child : ctx.storage.nodes.get_args_view(node))
      if (readsBinding(ctx, child, target))
        return true;
    return false;
  }
};

//
// Machinery
//
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

  // Deliberately a live replay rather than the solver's recorded boundaries.
  // EffectPropPass rewrites expression trees as it sweeps and inlines reads
  // immediately, so it has to see the facts about the trees as they are now,
  // not as they were when the analysis ran. Frozen states let it inline a read
  // and then erase the definition that read still depends on.
  EffectAtStmtState getStateBeforeStatement(const IRICFG &cfg, BBIDX block,
                                            size_t offset) const {
    EffectAtStmtState state = solver->getBlockEntryState(block);
    const std::vector<IRID> &stmts = cfg.at(block).stmts;
    for (size_t i = 0; i < offset; i++)
      state = transfer->transferStatement(cfg.ctx, stmts.at(i), state);
    return state;
  }
};

struct EffectAtStmtAnalysis {
  static inline char ID = 0;
  using Result = EffectAtStmtResult;

  EffectAtStmtResult run(IRICFG &cfg, AnalysisManager &am) {
    auto transfer = std::make_shared<EffectAtStmtTransfer>();
    auto solver =
        std::make_shared<DataflowSolver<EffectAtStmtState>>(cfg, *transfer);

    // No statement has run yet, so nothing is current at ENTRY. Every other
    // block starts UNKNOWN, the join identity: a block has to adopt whatever
    // its predecessors report rather than refuse it.
    solver->run(EffectAtStmtState(EffectAtStmtState::NOEFFECT),
                EffectAtStmtState(EffectAtStmtState::UNKNOWN));

    return EffectAtStmtResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
