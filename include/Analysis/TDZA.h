#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/IRIS.hpp"
#include <cassert>
#include <memory>
#include <vector>

namespace IRI_STRUCTURAL {

//
// Lattice
//
struct TDZLattice {
  enum Kind { TOP, TDZ, SAFE };
  Kind kind = TOP;

  TDZLattice(Kind k) : kind(k) {}

  [[nodiscard]] TDZLattice joinWith(const TDZLattice &other) const {
    if (kind == TOP)
      return other;
    if (other.kind == TOP)
      return *this;
    return kind == TDZ || other.kind == TDZ ? TDZLattice(TDZ)
                                            : TDZLattice(SAFE);
  }

  bool operator==(const TDZLattice &other) const { return kind == other.kind; }
};

//
// Lattice = Lattice X Bindings
//
class TDZState {
private:
  ImmutableDataMap<IRID, TDZLattice> envBindings;

public:
  TDZState() = default;
  explicit TDZState(ImmutableDataMap<IRID, TDZLattice> bindings)
      : envBindings(std::move(bindings)) {}

  static TDZState over(const std::vector<IRID> &bindings,
                       const TDZLattice &initial) {
    ImmutableDataMap<IRID, TDZLattice> map;
    for (IRID bID : bindings)
      map = map.set(bID, initial);
    return TDZState(map);
  }

  [[nodiscard]] TDZState joinWith(const TDZState &other) const {
    if (*this == other)
      return *this;

    return TDZState(envBindings.joinWith(other.envBindings));
  }

  bool operator==(const TDZState &other) const {
    return envBindings == other.envBindings;
  }

  TDZLattice getLattice(IRID binding) const { return envBindings.get(binding); }

  TDZState setLattice(IRID binding, TDZLattice val) const {
    assert(envBindings.contains(binding));
    return TDZState(envBindings.set(binding, val));
  }
};

//
// Transfer Function
//
class TDZATransfer : public TransferFunction<TDZState> {
public:
  TDZState transferStatement(IRIContext &ctx, IRID stmt,
                             const TDZState &incomingState) override {
    TDZState nextState = incomingState;

    switch (IRI_NODE(ctx, stmt).tag) {
    // Writing NUBD is the declaration's initialization; any other value makes
    // the binding live from here on.
    case IRI_GEN::LWrite: {
      IRI_GEN::LWriteSEXP lw(stmt, ctx);
      IRID target = lw.getArg_LValTarget();
      if (IRI_NODE(ctx, target).tag != IRI_GEN::EnvBinding)
        break;
      bool nubd = IRI_NODE(ctx, lw.getArg_RVal()).tag == IRI_GEN::JSNUBD;
      nextState = nextState.setLattice(
          target, TDZLattice(nubd ? TDZLattice::TDZ : TDZLattice::SAFE));
      break;
    }
    // args[0] is the read; the rest are the writes it feeds.
    case IRI_GEN::CompoundAssn: {
      auto args = ctx.storage.nodes.get_args(stmt);
      for (size_t i = 1; i < args.size(); i++)
        if (IRI_NODE(ctx, args[i]).tag == IRI_GEN::LWrite) {
          IRID target = IRI_GEN::LWriteSEXP(args[i], ctx).getArg_LValTarget();
          if (IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding)
            nextState =
                nextState.setLattice(target, TDZLattice(TDZLattice::SAFE));
        }
      break;
    }
    default:
      break;
    }

    return nextState;
  }
};

//
// Machinery
//
class TDZAnalysisResult : public DataflowResultConcept {
private:
  std::shared_ptr<TDZATransfer> transfer;
  std::shared_ptr<DataflowSolver<TDZState>> solver;

public:
  TDZAnalysisResult() = default;
  TDZAnalysisResult(std::shared_ptr<TDZATransfer> t,
                    std::shared_ptr<DataflowSolver<TDZState>> s)
      : transfer(std::move(t)), solver(std::move(s)) {}

  std::string getAnalysisName() const override { return "TDZ"; }

  const TDZState &getStateBeforeStatement(BBIDX block,
                                          size_t stmtOffset) const {
    return solver->getStateBeforeStatement(block, stmtOffset);
  }
};

struct TDZAnalysis {
  static inline char ID = 0;
  using Result = TDZAnalysisResult;

  TDZAnalysisResult run(IRICFG &cfg, AnalysisManager &am) {
    auto transfer = std::make_shared<TDZATransfer>();
    auto solver = std::make_shared<DataflowSolver<TDZState>>(cfg, *transfer);

    SCPIDX headScope = cfg.at(cfg.entry).SCOPE;
    auto bindings = cfg.ctx.iris->getEnvBindingsInClosure(headScope);
    TDZState init = TDZState::over(bindings, TDZLattice(TDZLattice::TOP));

    // Nothing is known about any binding at entry, so the boundary state is
    // INIT itself.
    solver->run(init, init);

    return TDZAnalysisResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
