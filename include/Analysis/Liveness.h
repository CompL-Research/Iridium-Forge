#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/BBContainerSupport.hpp"
#include "Support/IRIS.hpp"
#include <memory>
#include <vector>

namespace IRI_STRUCTURAL {

//
// Lattice
//
struct LivenessLattice {
  enum Kind { DEAD, LIVE };
  Kind kind = DEAD;

  LivenessLattice(Kind k) : kind(k) {}

  [[nodiscard]] LivenessLattice joinWith(const LivenessLattice &other) const {
    return kind == LIVE || other.kind == LIVE ? LivenessLattice(LIVE)
                                              : LivenessLattice(DEAD);
  }

  bool operator==(const LivenessLattice &other) const {
    return kind == other.kind;
  }
};

//
// Lattice = Lattice X Bindings
//
class LivenessState {
private:
  ImmutableDataMap<IRID, LivenessLattice> liveBindings;

public:
  LivenessState() = default;
  explicit LivenessState(ImmutableDataMap<IRID, LivenessLattice> bindings)
      : liveBindings(std::move(bindings)) {}

  static LivenessState over(const std::vector<IRID> &bindings,
                            const LivenessLattice &initial) {
    ImmutableDataMap<IRID, LivenessLattice> map;
    for (IRID bID : bindings)
      map = map.set(bID, initial);
    return LivenessState(map);
  }

  [[nodiscard]] LivenessState joinWith(const LivenessState &other) const {
    if (*this == other)
      return *this;

    return LivenessState(liveBindings.joinWith(other.liveBindings));
  }

  bool operator==(const LivenessState &other) const {
    return liveBindings == other.liveBindings;
  }

  LivenessLattice getLattice(IRID binding) const {
    return liveBindings.get(binding);
  }

  LivenessState setLattice(IRID binding, LivenessLattice val) const {
    assert(liveBindings.contains(binding));
    return LivenessState(liveBindings.set(binding, val));
  }
};

//
// Transfer Function
//
class LivenessTransfer : public TransferFunction<LivenessState> {
private:
  bool isStrict;

public:
  explicit LivenessTransfer(bool strict) : isStrict(strict) {}

  // A binding this analysis may never kill: eval or a nested closure can read
  // it, and in a non-strict function a formal parameter can be read implicitly.
  bool alwaysLive(IRIContext &ctx, IRID binding) const {
    if (IRI_NODE(ctx, binding).tag != IRI_GEN::EnvBinding)
      return false;
    auto &iris = ctx.iris;
    return IRI_BINDING(iris, binding).isEvalTainted() ||
           IRI_BINDING(iris, binding).isCaptured() ||
           (!isStrict && IRI_GEN::EnvBindingSEXP(binding, ctx).hasJSARG());
  }

  LivenessState transferStatement(IRIContext &ctx, IRID stmt,
                                  const LivenessState &incomingState) override {
    LivenessState nextState = incomingState;

    // Kill.
    if (IRI_NODE(ctx, stmt).tag == IRI_GEN::LWrite) {
      IRI_GEN::LWriteSEXP lw(stmt, ctx);
      IRID target = lw.getArg_LValTarget();
      // Only an initializing, this-initializing or checked write definitely
      // stores, so only those can kill.
      if ((lw.hasINIT() || lw.hasTHISINIT() || lw.hasSAFE()) &&
          IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding) {
        if (!alwaysLive(ctx, target))
          nextState =
              nextState.setLattice(target, LivenessLattice(LivenessLattice::DEAD));
      }
    }

    // Gen.
    markLive(ctx, nextState, stmt);

    return nextState;
  }

private:
  void markLive(IRIContext &ctx, LivenessState &state, IRID node) {
    if (IRI_NODE(ctx, node).tag == IRI_GEN::EnvRead) {
      IRID target = IRI_GEN::EnvReadSEXP(node, ctx).getArg_Obj();
      if (IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding)
        state = state.setLattice(target, LivenessLattice(LivenessLattice::LIVE));
    }
    for (IRID child : ctx.storage.nodes.get_args_view(node))
      markLive(ctx, state, child);
  }
};

//
// Machinery
//
class LivenessAnalysisResult : public DataflowResultConcept {
private:
  std::shared_ptr<LivenessTransfer> transfer;
  std::shared_ptr<BackwardDataflowSolver<LivenessState>> solver;

public:
  LivenessAnalysisResult() = default;
  LivenessAnalysisResult(
      std::shared_ptr<LivenessTransfer> t,
      std::shared_ptr<BackwardDataflowSolver<LivenessState>> s)
      : transfer(std::move(t)), solver(std::move(s)) {}

  std::string getAnalysisName() const override { return "Liveness"; }

  const LivenessState &getStateAfterStatement(BBIDX block,
                                              size_t stmtOffset) const {
    return solver->getStateAfterStatement(block, stmtOffset);
  }

  const LivenessState &getBlockEntryState(BBIDX block) const {
    return solver->getBlockEntryState(block);
  }
};

struct LivenessAnalysis {
  static inline char ID = 0;
  using Result = LivenessAnalysisResult;

  LivenessAnalysisResult run(IRICFG &cfg, AnalysisManager &am) {
    auto &ctx = cfg.ctx;
    bool strict = BBContainerSupport(cfg.container, ctx).hasSTRICT();

    auto transfer = std::make_shared<LivenessTransfer>(strict);
    auto solver =
        std::make_shared<BackwardDataflowSolver<LivenessState>>(cfg, *transfer);

    SCPIDX headScope = cfg.at(cfg.entry).SCOPE;
    auto bindings = ctx.iris->getEnvBindingsInClosure(headScope);
    LivenessState state =
        LivenessState::over(bindings, LivenessLattice(LivenessLattice::DEAD));
    for (IRID bID : bindings)
      if (transfer->alwaysLive(ctx, bID))
        state = state.setLattice(bID, LivenessLattice(LivenessLattice::LIVE));

    solver->run(state, state);

    return LivenessAnalysisResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
