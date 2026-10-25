#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/BBContainerSupport.hpp"
#include "Support/IRIS.hpp"
#include <iostream>
#include <memory>

namespace IRI_STRUCTURAL {

// ============================================================================
// 1. TDZ Lattice Definition
// ============================================================================
struct TDZLattice {
  enum Kind { TOP, TDZ, SAFE };

  Kind kind = TOP;

  TDZLattice() = default;
  TDZLattice(Kind k) : kind(k) {}

  static TDZLattice bottom() { return TDZLattice(TDZ); }
  static TDZLattice top() { return TDZLattice(TOP); }

  // Lattice join operator (Least Upper Bound ⊔)
  [[nodiscard]] TDZLattice joinWith(const TDZLattice &other) const {
    if (kind == TOP) {
      return other;
    }
    if (other.kind == TOP) {
      return *this;
    }
    // If either path could be TDZ, the result must be TDZ (over-approximation)
    if (kind == TDZ || other.kind == TDZ) {
      return TDZLattice(TDZ);
    }
    return TDZLattice(SAFE);
  }

  bool operator==(const TDZLattice &other) const { return kind == other.kind; }
};

// ============================================================================
// 2. TDZ Dataflow State
// ============================================================================
class TDZState {
private:
  bool is_unreachable = true;
  ImmutableDataMap<IRID, TDZLattice> envBindings;

public:
  // Default constructor creates an unreachable/unvisited state
  TDZState() : is_unreachable(true) {}
  explicit TDZState(ImmutableDataMap<IRID, TDZLattice> bindings,
                    bool unreachable = false)
      : envBindings(std::move(bindings)), is_unreachable(unreachable) {}

  // Solver-required interface: Bottom of the join-semilattice represents
  // unvisited/unreachable
  static TDZState bottom() {
    return TDZState(); // Default constructor creates unreachable state
  }

  static TDZState reachableEmpty() {
    return TDZState(ImmutableDataMap<IRID, TDZLattice>(), false);
  }

  bool isUnreachable() const { return is_unreachable; }

  void dump(IRI_STORAGE::IRIContext &ctx, std::ostream &oss) const {
    if (is_unreachable) {
      oss << "<Unreachable>";
      return;
    }
    oss << "{";
    bool first = true;
    for (const auto &[bID, lattice] : envBindings.getRawMap()) {
      if (!first)
        oss << ", ";
      first = false;
      if (IRI_NODE(ctx, bID).tag == IRI_GEN::EnvBinding) {
        IRI_GEN::EnvBindingSEXP eb(bID, ctx);
        oss << ctx.storage.strings.get(eb.getNAME());
      } else {
        oss << "IRID(" << bID << ")";
      }
      oss << ": ";
      if (lattice.kind == TDZLattice::TOP)
        oss << "TOP";
      else if (lattice.kind == TDZLattice::TDZ)
        oss << "TDZ";
      else if (lattice.kind == TDZLattice::SAFE)
        oss << "SAFE";
    }
    oss << "}";
  }

  // Lattice join for states
  [[nodiscard]] TDZState joinWith(const TDZState &other) const {
    if (is_unreachable)
      return other;
    if (other.is_unreachable)
      return *this;
    if (*this == other)
      return *this;

    return TDZState(envBindings.joinWith(other.envBindings), false);
  }

  bool operator==(const TDZState &other) const {
    if (is_unreachable != other.is_unreachable)
      return false;
    if (is_unreachable)
      return true;
    return envBindings == other.envBindings;
  }

  // Get Lattice value for a binding
  TDZLattice getLattice(IRID binding) const {
    return envBindings.get(binding, TDZLattice::top());
  }

  // Immutable update: returns a new state with binding mapped to value
  TDZState setLattice(IRID binding, TDZLattice val) const {
    return TDZState(envBindings.set(binding, val), false);
  }
};

// ============================================================================
// 3. TDZ Transfer Function
// ============================================================================
class TDZATransfer : public TransferFunction<TDZState> {
public:
  TDZState transferStatement(const IRIStatement &stmt,
                             const TDZState &incomingState) override {
    if (incomingState.isUnreachable()) {
      return TDZState::bottom(); // Unreachable block state propagation
    }

    TDZState nextState = incomingState;

    auto &ctx = stmt.bb->ctx;
    auto tag = IRI_NODE(ctx, stmt.id).tag;
    // Predicates can probably be much simpler,
    // by construction setting a value to NUBD will
    // be at scope boundaries anyway.
    // So any write, which is not NUBD just transitions the state to SAFE.
    // Even if its a write in TDZ zone, subsequent writes can be assumed as safe
    // because the check would need to pass for the following code to be valid
    // anyway.
    if (tag == IRI_GEN::LWrite) {
      IRI_GEN::LWriteSEXP lw(stmt.id, ctx);

      auto rval = lw.getArg_RVal();
      auto lval = lw.getArg_LValTarget();
      if (IRI_NODE(ctx, rval).tag == IRI_GEN::JSNUBD) {
        nextState = nextState.setLattice(lval, TDZLattice(TDZLattice::TDZ));
      } else {
        nextState = nextState.setLattice(lval, TDZLattice(TDZLattice::SAFE));
      }

      // if (lw.hasINIT()) {
      //   auto rval = lw.getArg_RVal();
      //   auto lval = lw.getArg_LValTarget();
      //   if (IRI_NODE(ctx, rval).tag == IRI_GEN::JSNUBD) {
      //     nextState = nextState.setLattice(lval,
      //     TDZLattice(TDZLattice::TDZ));
      //   } else {
      //     nextState = nextState.setLattice(lval,
      //     TDZLattice(TDZLattice::SAFE));
      //   }
      // }
    } else if (tag == IRI_GEN::CompoundAssn) {
      auto args = ctx.storage.nodes.get_args(stmt.id);
      assert(args.size() > 1);
      IRID rval = args[0];
      for (size_t i = 1; i < args.size(); i++) {
        IRID currWriteID = args[i];
        if (IRI_NODE(ctx, currWriteID).tag == IRI_GEN::LWrite) {
          IRI_GEN::LWriteSEXP lw(currWriteID, ctx);

          auto lval = lw.getArg_LValTarget();
          nextState = nextState.setLattice(lval, TDZLattice(TDZLattice::SAFE));

          // if (lw.hasINIT()) {
          //   auto lval = lw.getArg_LValTarget();
          //   if (IRI_NODE(ctx, rval).tag == IRI_GEN::JSNUBD) {
          //     nextState = nextState.setLattice(lval,
          //     TDZLattice(TDZLattice::TDZ));
          //   } else {
          //     nextState = nextState.setLattice(lval,
          //     TDZLattice(TDZLattice::SAFE));
          //   }
          // }
        }
      }
    }

    return nextState;
  }

  TDZState transferEdge(BBIDX from, BBIDX to,
                        const TDZState &exitState) override {
    // Optionally refine state based on conditional branch targets
    return exitState;
  }
};

// ============================================================================
// 4. TDZ Analysis Result and Pass Definition
// ============================================================================
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

  void dumpStateAtStatement(const IRIStatement &stmt,
                            IRI_STORAGE::IRIContext &ctx,
                            std::ostream &os) const override {
    TDZState state = queryStateAtStatement(stmt);
    state.dump(ctx, os);
  }

  void dumpBlockEntryState(BBIDX block, IRI_STORAGE::IRIContext &ctx,
                           std::ostream &os) const override {
    const TDZState &state = getBlockEntryState(block);
    state.dump(ctx, os);
  }

  void dumpBlockExitState(BBIDX block, IRI_STORAGE::IRIContext &ctx,
                          std::ostream &os) const override {
    os << "<ExitStateNotTracked>";
  }

  const TDZState &getBlockEntryState(BBIDX block) const {
    return solver->getBlockEntryState(block);
  }

  TDZState queryStateAtStatement(const IRIStatement &stmt) const {
    return solver->queryStateAtStatement(stmt);
  }
};

struct TDZAnalysis {
  static inline char ID = 0;
  using Result = TDZAnalysisResult;

  TDZAnalysisResult run(IRICFG &cfg, AnalysisManager &am) {
    auto transfer = std::make_shared<TDZATransfer>();
    auto solver = std::make_shared<DataflowSolver<TDZState>>(cfg, *transfer);

    // Initial State
    double headScope = cfg.nodeMap.at(cfg.entry_block)->SCOPE;
    auto bindings = cfg.ctx.iris->getEnvBindingsInClosure(headScope);
    TDZState entryState = TDZState::reachableEmpty();
    for (auto bID : bindings) {
      entryState = entryState.setLattice(bID, TDZLattice::top());
    }

    // Run the solver to fixed-point, including exceptional edges for soundness
    solver->run(entryState, /*includeExceptions=*/true);

    return TDZAnalysisResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
