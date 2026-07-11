#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/BBContainerSupport.hpp"
#include "Support/IRIS.hpp"
#include <iostream>
#include <memory>

#define DEBUG_LIVENESS 0

namespace IRI_STRUCTURAL {

// ============================================================================
// 1. Liveness Lattice Definition
// ============================================================================
struct LivenessLattice {
  enum Kind {
    TOP,  // Unknown / unvisited state
    DEAD, // Optimistic state
    LIVE  // Bottom / most generic (conservative) state
  };

  Kind kind = TOP;

  LivenessLattice() = default;
  LivenessLattice(Kind k) : kind(k) {}

  static LivenessLattice bottom() { return LivenessLattice(LIVE); }
  static LivenessLattice top() { return LivenessLattice(TOP); }

  // Lattice join operator (Least Upper Bound ⊔)
  void joinWith(const LivenessLattice &other) {
    if (kind == TOP) {
      kind = other.kind;
      return;
    }
    if (other.kind == TOP) {
      return;
    }
    // If either path could be LIVE, the result must be LIVE
    // (conservative/bottom)
    if (kind == LIVE || other.kind == LIVE) {
      kind = LIVE;
    } else {
      kind = DEAD;
    }
  }

  bool operator==(const LivenessLattice &other) const {
    return kind == other.kind;
  }
};

// ============================================================================
// 2. Liveness Dataflow State
// ============================================================================
class LivenessState {
private:
  bool is_unreachable = true;
  ImmutableDataMap<IRID, LivenessLattice> liveBindings;

public:
  LivenessState() : is_unreachable(true) {}
  explicit LivenessState(ImmutableDataMap<IRID, LivenessLattice> bindings,
                         bool unreachable = false)
      : liveBindings(std::move(bindings)), is_unreachable(unreachable) {}

  static LivenessState bottom() {
    return LivenessState(); // Default constructor creates unreachable/unvisited
                            // state
  }

  static LivenessState reachableEmpty() {
    return LivenessState(ImmutableDataMap<IRID, LivenessLattice>(), false);
  }

  bool isUnreachable() const { return is_unreachable; }

  // Lattice join for liveness states
  LivenessState joinWith(const LivenessState &other) const {
    if (is_unreachable)
      return other;
    if (other.is_unreachable)
      return *this;

    return LivenessState(liveBindings.joinWith(other.liveBindings), false);
  }

  bool operator==(const LivenessState &other) const {
    if (is_unreachable != other.is_unreachable)
      return false;
    if (is_unreachable)
      return true;
    return liveBindings == other.liveBindings;
  }

  LivenessLattice getLattice(IRID binding) const {
    return liveBindings.get(binding, LivenessLattice::top()); // Default is TOP (Unknown)
  }

  LivenessState setLattice(IRID binding, LivenessLattice val) const {
    return LivenessState(liveBindings.set(binding, val), false);
  }

  void dump(IRI_STORAGE::IridiumPool &pool, std::ostream &oss) const {
    if (is_unreachable) {
      oss << "<Unreachable>";
      return;
    }
    oss << "{";
    bool first = true;
    for (const auto &[bID, lattice] : liveBindings.getRawMap()) {
      if (!first)
        oss << ", ";
      first = false;
      if (pool[bID].tag == IRI_GEN::EnvBinding) {
        IRI_GEN::EnvBindingSEXP eb(bID, pool);
        oss << pool.strings.get(eb.getNAME());
      } else {
        oss << "IRID(" << bID << ")";
      }
      oss << ": ";
      if (lattice.kind == LivenessLattice::TOP)
        oss << "TOP";
      else if (lattice.kind == LivenessLattice::DEAD)
        oss << "DEAD";
      else if (lattice.kind == LivenessLattice::LIVE)
        oss << "LIVE";
    }
    oss << "}";
  }
};

// ============================================================================
// 3. Liveness Transfer Function
// ============================================================================
class LivenessTransfer : public TransferFunction<LivenessState> {
private:
  bool isStrict;
public:
  explicit LivenessTransfer(bool strict) : isStrict(strict) {}

  LivenessState transferStatement(const IRIStatement &stmt,
                                  const LivenessState &incomingState) override {
    if (incomingState.isUnreachable()) {
      return LivenessState::bottom();
    }

    LivenessState nextState = incomingState;
    auto &pool = stmt.bb->pool;
    auto &iris = pool.iris;
    auto tag = pool[stmt.id].tag;

    // Kill (if not captured, and not a non-strict formal parameter)
    if (tag == IRI_GEN::LWrite) {
      LWriteSEXP lw(stmt.id, pool);
      auto target = lw.getArg_LValTarget();
      //
      // Deprecated -> ThisINITSEXP [RVAL], basically it takes two arguments (first is an EnvRead to 'this' and second is the new value for 'this') node makes this dependency explicit
      //
      // If it is a THISINIT write, it checks the TDZ state of the target,
      // so it acts as a read (making the target LIVE) rather than a kill.
      // if (lw.hasTHISINIT()) {
      //   if (pool[target].tag == IRI_GEN::EnvBinding) {
      //     nextState = nextState.setLattice(target, LivenessLattice::LIVE);
      //   }
      // } else

      if (lw.hasTHISINIT() || lw.hasINIT() || lw.hasSAFE()) {
        if (pool[target].tag == IRI_GEN::EnvBinding) {
          EnvBindingSEXP eb(target, pool);
          bool isEvalTainted = (*iris)[target].isEvalTainted();
          bool isCap = (*iris)[target].isCaptured();
          bool isArg = eb.hasJSARG();
          bool alwaysLive = isEvalTainted || isCap || (!isStrict && isArg);
          if (!alwaysLive) {
            nextState = nextState.setLattice(target, LivenessLattice::DEAD);
          }
        }
      }
    }

    // Gen
    auto processNestedEnvReads = [&](auto &self, IRID node) -> void {
      if (pool[node].tag == IRI_GEN::EnvRead) {
        IRI_GEN::EnvReadSEXP envRead(node, pool);
        auto target = envRead.getArg_Obj();

        // Reading a local stack binding
        if (pool[target].tag == IRI_GEN::EnvBinding) {
          nextState = nextState.setLattice(target, LivenessLattice::LIVE);
        }
      }
      for (auto child : pool.get_args_view(node)) {
        self(self, child);
      }
    };
    processNestedEnvReads(processNestedEnvReads, stmt.id);

    return nextState;
  }

  LivenessState transferEdge(BBIDX from, BBIDX to,
                             const LivenessState &exitState) override {
    return exitState;
  }
};

// ============================================================================
// 4. Liveness Analysis Result
// ============================================================================
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

  void dumpStateAtStatement(const IRIStatement &stmt,
                            IRI_STORAGE::IridiumPool &pool,
                            std::ostream &os) const override {
    LivenessState state = solver->getBlockExitState(stmt.bb->IDX);
    if (stmt.bb->tail) {
      state = transfer->transferStatement(*stmt.bb->tail, state);
    }
    std::vector<IRIStatement *> stmts;
    for (IRIStatement *s = stmt.bb->head; s != nullptr; s = s->next) {
      stmts.push_back(s);
    }
    for (auto it = stmts.rbegin(); it != stmts.rend(); ++it) {
      if (*it == &stmt) {
        LivenessState stateBefore = transfer->transferStatement(**it, state);
        stateBefore.dump(pool, os);
        return;
      }
      state = transfer->transferStatement(**it, state);
    }
    state.dump(pool, os);
  }

  void dumpBlockEntryState(BBIDX block, IRI_STORAGE::IridiumPool &pool,
                           std::ostream &os) const override {
    const LivenessState &state = getBlockEntryState(block);
    state.dump(pool, os);
  }

  void dumpBlockExitState(BBIDX block, IRI_STORAGE::IridiumPool &pool,
                          std::ostream &os) const override {
    const LivenessState &state = getBlockExitState(block);
    state.dump(pool, os);
  }

  const LivenessState &getBlockEntryState(BBIDX block) const {
    return solver->getBlockEntryState(block);
  }

  const LivenessState &getBlockExitState(BBIDX block) const {
    return solver->getBlockExitState(block);
  }
};

struct LivenessAnalysis {
  static inline char ID = 0;
  using Result = LivenessAnalysisResult;

  LivenessAnalysisResult run(IRICFG &cfg, AnalysisManager &am) {
    auto &pool = cfg.pool;
    auto &iris = pool.iris;
    BBContainerSupport bbc(cfg.id, pool);
    bool strict = bbc.hasSTRICT();

    auto transfer = std::make_shared<LivenessTransfer>(strict);
    auto solver =
        std::make_shared<BackwardDataflowSolver<LivenessState>>(cfg, *transfer);

    // Initial State
    double headScope = bbc.getScopeIDX();
    auto bindings = iris->getEnvBindingsInClosure(headScope);
    LivenessState exitState = LivenessState::reachableEmpty();
    for (auto bID : bindings) {
      EnvBindingSEXP eb(bID, pool);
      bool isEvalTainted = (*iris)[bID].isEvalTainted();
      bool isCap = (*iris)[bID].isCaptured();
      bool isArg = eb.hasJSARG();
      bool alwaysLive = isEvalTainted || isCap || (!strict && isArg);
      exitState = exitState.setLattice(bID, alwaysLive
                                                ? LivenessLattice::bottom() // LIVE
                                                : LivenessLattice(LivenessLattice::DEAD));
    }

#if DEBUG_LIVENESS
    BBContainerSupport bbc(cfg.id, cfg.pool);
    std::cout << "  [Liveness-Analysis] Starting backward solver run on CFG of "
                 "closure "
              << bbc.getStartBBIDX() << "\n";
#endif

    solver->run(exitState, /*includeExceptions=*/true);

    return LivenessAnalysisResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
