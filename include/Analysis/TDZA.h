#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/IRIS.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace IRI_STRUCTURAL {

inline bool trackedBinding(IRI_STORAGE::IRIContext &ctx, IRID binding) {
  IRI_TAG tag = IRI_NODE(ctx, binding).tag;
  return tag == IRI_GEN::EnvBinding || tag == IRI_GEN::RemoteEnvBinding;
}

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
    return (kind == TDZ || other.kind == TDZ) ? TDZLattice(TDZ)
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

  static TDZState over(const std::vector<IRID> &locals,
                       const std::vector<IRID> &remotes, TDZLattice localSeed,
                       TDZLattice remoteSeed) {
    ImmutableDataMap<IRID, TDZLattice> map;
    for (IRID b : locals)
      map = map.set(b, localSeed);
    for (IRID b : remotes)
      map = map.set(b, remoteSeed);
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

  TDZLattice getLattice(IRID binding) const {
    if (!envBindings.contains(binding))
      throw std::runtime_error("TDZA: no lattice for binding " +
                               std::to_string(binding));
    return envBindings.get(binding);
  }

  TDZState setLattice(IRID binding, TDZLattice val) const {
    if (!envBindings.contains(binding))
      throw std::runtime_error("TDZA: binding " + std::to_string(binding) +
                               " is not declared in this closure");
    return TDZState(envBindings.set(binding, val));
  }
};

//
// Transfer Function
//
class TDZATransfer : public TransferFunction<TDZState> {
public:
  static void markReads(IRIContext &ctx, IRID node, TDZState &state) {
    if (IRI_NODE(ctx, node).tag == IRI_GEN::EnvRead) {
      IRID obj = ctx.storage.nodes.get_args_view(node)[0];
      if (trackedBinding(ctx, obj))
        state = state.setLattice(obj, TDZLattice(TDZLattice::SAFE));
    }
    for (IRID child : ctx.storage.nodes.get_args_view(node))
      markReads(ctx, child, state);
  }

  TDZState transferStatement(IRIContext &ctx, IRID stmt,
                             const TDZState &incomingState) override {
    TDZState nextState = incomingState;

    markReads(ctx, stmt, nextState);

    switch (IRI_NODE(ctx, stmt).tag) {
    case IRI_GEN::LWrite:
    case IRI_GEN::RWrite:
    case IRI_GEN::MWrite: {
      auto args = ctx.storage.nodes.get_args_view(stmt);
      bool nubd = IRI_NODE(ctx, args[1]).tag == IRI_GEN::JSNUBD;
      nextState = nextState.setLattice(
          args[0], TDZLattice(nubd ? TDZLattice::TDZ : TDZLattice::SAFE));
      break;
    }
    case IRI_GEN::CompoundAssn: {
      auto args = ctx.storage.nodes.get_args(stmt);
      for (size_t i = 1; i < args.size(); i++) {
        IRI_TAG innerTag = IRI_NODE(ctx, args[i]).tag;
        if (innerTag != IRI_GEN::LWrite && innerTag != IRI_GEN::RWrite &&
            innerTag != IRI_GEN::MWrite)
          continue;
        IRID target = ctx.storage.nodes.get_args_view(args[i])[0];
        nextState = nextState.setLattice(target, TDZLattice(TDZLattice::SAFE));
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
    auto locals = cfg.ctx.iris->getEnvBindingsInClosure(headScope);
    auto remotes = cfg.ctx.iris->getRemoteEnvBindingsInClosure(headScope);
    TDZState boundary =
        TDZState::over(locals, remotes, TDZLattice(TDZLattice::SAFE),
                       TDZLattice(TDZLattice::TDZ));
    TDZState top = TDZState::over(locals, remotes, TDZLattice(TDZLattice::TOP),
                                  TDZLattice(TDZLattice::TOP));

    solver->run(boundary, top);

    return TDZAnalysisResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
