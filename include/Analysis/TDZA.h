#pragma once

#include "Support/AbstractInterpretation.hpp"
#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/IRIS.hpp"
#include "Support/BBContainerSupport.hpp"
#include <iostream>
#include <memory>

namespace IRI_STRUCTURAL {

// ============================================================================
// 1. TDZ Lattice Definition
// ============================================================================
struct TDZLattice {
  enum Kind {
    TOP,
    TDZ,
    SAFE
  };

  Kind kind = TOP;

  TDZLattice() = default;
  TDZLattice(Kind k) : kind(k) {}

  static TDZLattice bottom() { return TDZLattice(TDZ); }
  static TDZLattice top() { return TDZLattice(TOP); }

  // Lattice join operator (Least Upper Bound ⊔)
  void joinWith(const TDZLattice& other) {
    if (kind == TOP) {
      kind = other.kind;
      return;
    }
    if (other.kind == TOP) {
      return;
    }
    // If either path could be TDZ, the result must be TDZ (over-approximation)
    if (kind == TDZ || other.kind == TDZ) {
      kind = TDZ;
    } else {
      kind = SAFE;
    }
  }

  bool operator==(const TDZLattice& other) const {
    return kind == other.kind;
  }
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
  explicit TDZState(ImmutableDataMap<IRID, TDZLattice> bindings, bool unreachable = false)
      : envBindings(std::move(bindings)), is_unreachable(unreachable) {}

  // Solver-required interface: Bottom of the join-semilattice represents unvisited/unreachable
  static TDZState bottom() {
    return TDZState(); // Default constructor creates unreachable state
  }

  static TDZState reachableEmpty() {
    return TDZState(ImmutableDataMap<IRID, TDZLattice>(), false);
  }

  bool isUnreachable() const { return is_unreachable; }

  void dump(IRI_STORAGE::IridiumPool &pool, std::ostream &oss) const {
    if (is_unreachable) {
      oss << "<Unreachable>";
      return;
    }
    oss << "{";
    bool first = true;
    for (const auto& [bID, lattice] : envBindings.getRawMap()) {
      if (!first) oss << ", ";
      first = false;
      if (pool[bID].tag == IRI_GEN::EnvBinding) {
        IRI_GEN::EnvBindingSEXP eb(bID, pool);
        oss << pool.strings.get(eb.getNAME());
      } else {
        oss << "IRID(" << bID << ")";
      }
      oss << ": ";
      if (lattice.kind == TDZLattice::TOP) oss << "TOP";
      else if (lattice.kind == TDZLattice::TDZ) oss << "TDZ";
      else if (lattice.kind == TDZLattice::SAFE) oss << "SAFE";
    }
    oss << "}";
  }

  // Lattice join for states
  TDZState joinWith(const TDZState& other) const {
    if (is_unreachable) return other;
    if (other.is_unreachable) return *this;

    return TDZState(envBindings.joinWith(other.envBindings), false);
  }

  bool operator==(const TDZState& other) const {
    if (is_unreachable != other.is_unreachable) return false;
    if (is_unreachable) return true;
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
// 3. TDZ Transfer Function (Boilerplate)
// ============================================================================
class TDZATransfer : public TransferFunction<TDZState> {
public:
  TDZState transferStatement(const IRIStatement& stmt, const TDZState& incomingState) override {
    if (incomingState.isUnreachable()) {
      return TDZState::bottom(); // Unreachable block state propagation
    }

    TDZState nextState = incomingState;

    auto &pool = stmt.bb->pool;
    auto tag = pool[stmt.id].tag;
    if (tag == IRI_GEN::LWrite) {
      IRI_GEN::LWriteSEXP lw(stmt.id, pool);
      if (lw.hasINIT()) {
        auto rval = lw.getArg_RVal();
        auto lval = lw.getArg_LValTarget();
        if (pool[rval].tag == IRI_GEN::JSNUBD) {
          nextState = nextState.setLattice(lval, TDZLattice(TDZLattice::TDZ));
        } else {
          nextState = nextState.setLattice(lval, TDZLattice(TDZLattice::SAFE));
        }
#if DEBUG_TDZ
        std::cout << "      [TDZA-Transfer] LWrite INIT stmt ID: " << stmt.id << " target: ";
        if (pool[lval].tag == IRI_GEN::EnvBinding) {
          IRI_GEN::EnvBindingSEXP eb(lval, pool);
          std::cout << pool.strings.get(eb.getNAME());
        } else {
          std::cout << "IRID(" << lval << ")";
        }
        std::cout << ", RVal tag: " << IRI_GEN::dump_tag(pool[rval].tag) << " -> "
                  << (pool[rval].tag == IRI_GEN::JSNUBD ? "TDZ" : "SAFE") << "\n";
#endif
      }
    } else if (tag == IRI_GEN::CompoundAssn) {
      auto args = pool.get_args(stmt.id);
      assert(args.size() > 1);
      IRID rval = args[0];
      for (size_t i = 1; i < args.size(); i++) {
        IRID currWriteID = args[i];
        if (pool[currWriteID].tag == IRI_GEN::LWrite) {
          IRI_GEN::LWriteSEXP lw(currWriteID, pool);
          if (lw.hasINIT()) {
            auto lval = lw.getArg_LValTarget();
            if (pool[rval].tag == IRI_GEN::JSNUBD) {
              nextState = nextState.setLattice(lval, TDZLattice(TDZLattice::TDZ));
            } else {
              nextState = nextState.setLattice(lval, TDZLattice(TDZLattice::SAFE));
            }
#if DEBUG_TDZ
            std::cout << "      [TDZA-Transfer] CompoundAssn LWrite INIT stmt ID: " << stmt.id << " target: ";
            if (pool[lval].tag == IRI_GEN::EnvBinding) {
              IRI_GEN::EnvBindingSEXP eb(lval, pool);
              std::cout << pool.strings.get(eb.getNAME());
            } else {
              std::cout << "IRID(" << lval << ")";
            }
            std::cout << ", RVal tag: " << IRI_GEN::dump_tag(pool[rval].tag) << " -> "
                      << (pool[rval].tag == IRI_GEN::JSNUBD ? "TDZ" : "SAFE") << "\n";
#endif
          }
        }
      }
    }

    return nextState;
  }

  TDZState transferEdge(BBIDX from, BBIDX to, const TDZState& exitState) override {
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
  TDZAnalysisResult(std::shared_ptr<TDZATransfer> t, std::shared_ptr<DataflowSolver<TDZState>> s)
      : transfer(std::move(t)), solver(std::move(s)) {}

  std::string getAnalysisName() const override {
    return "TDZ";
  }

  void dumpStateAtStatement(const IRIStatement& stmt, IRI_STORAGE::IridiumPool& pool, std::ostream& os) const override {
    TDZState state = queryStateAtStatement(stmt);
    state.dump(pool, os);
  }

  void dumpBlockEntryState(BBIDX block, IRI_STORAGE::IridiumPool& pool, std::ostream& os) const override {
    const TDZState& state = getBlockEntryState(block);
    state.dump(pool, os);
  }

  void dumpBlockExitState(BBIDX block, IRI_STORAGE::IridiumPool& pool, std::ostream& os) const override {
    os << "<ExitStateNotTracked>";
  }

  const TDZState& getBlockEntryState(BBIDX block) const {
    return solver->getBlockEntryState(block);
  }

  TDZState queryStateAtStatement(const IRIStatement& stmt) const {
    return solver->queryStateAtStatement(stmt);
  }
};

struct TDZAnalysis {
  static inline char ID = 0;
  using Result = TDZAnalysisResult;

  TDZAnalysisResult run(IRICFG& cfg, AnalysisManager& am) {
    auto transfer = std::make_shared<TDZATransfer>();
    auto solver = std::make_shared<DataflowSolver<TDZState>>(cfg, *transfer);

    // Initial State
    double headScope = cfg.nodeMap.at(cfg.entry_block)->SCOPE;
    auto bindings = cfg.pool.iris->getEnvBindingsInClosure(headScope);
    TDZState entryState = TDZState::reachableEmpty();
    for (auto bID : bindings) {
      entryState = entryState.setLattice(bID, TDZLattice::top());
    }

    // Run the solver to fixed-point, including exceptional edges for soundness
#if DEBUG_TDZ
    BBContainerSupport bbc(cfg.id, cfg.pool);
    std::cout << "  [TDZA-Analysis] Starting solver run on CFG of closure " << bbc.getStartBBIDX() << "\n";
    std::cout << "    Entry block BB" << cfg.entry_block << " seed state: ";
    entryState.dump(cfg.pool, std::cout);
    std::cout << "\n";
#endif

    solver->run(entryState, /*includeExceptions=*/true);

#if DEBUG_TDZ
    std::cout << "  [TDZA-Analysis] Finished running TDZ Analysis solver on CFG of closure " << bbc.getStartBBIDX() << "\n";
    for (const auto& [idx, bb] : cfg.nodeMap) {
      const TDZState& state = solver->getBlockEntryState(idx);
      std::cout << "    BB" << idx << " entry state: ";
      state.dump(cfg.pool, std::cout);
      std::cout << "\n";
    }
#endif

    return TDZAnalysisResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
