#pragma once

#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "IRICFG.hpp"
#include <algorithm>
#include <cassert>
#include <immer/map.hpp>
#include <immer/map_transient.hpp>
#include <queue>
#include <set>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace IRI_STRUCTURAL {

// ============================================================================
// 1. ImmutableDataMap
// ============================================================================
// A wrapper around immer::map to provide immutable value-semantics,
// structural sharing, and O(log N) updates for dataflow state maps.
template <typename Key, typename LatticeElement> class ImmutableDataMap {
private:
  immer::map<Key, LatticeElement> data;

public:
  ImmutableDataMap() = default;
  explicit ImmutableDataMap(immer::map<Key, LatticeElement> d)
      : data(std::move(d)) {}

  // Returns a new map with the key set to the given lattice element (immutable
  // update)
  ImmutableDataMap set(const Key &key, const LatticeElement &val) const {
    return ImmutableDataMap(data.set(key, val));
  }

  // Returns a new map with f(key, value) applied to every entry. Used by kills
  // that have to test the value rather than the key.
  template <typename F> ImmutableDataMap mapValues(F f) const {
    auto result = data.transient();
    for (const auto &[key, val] : data)
      result.set(key, f(key, val));
    return ImmutableDataMap(result.persistent());
  }

  // Returns the value for a key, or the defaultValue if not found
  const LatticeElement &get(const Key &key,
                            const LatticeElement &defaultValue) const {
    if (auto ptr = data.find(key)) {
      return *ptr;
    }
    return defaultValue;
  }

  // The key must be present. A state is only ever as total as the INIT and
  // boundary states it was built from, so totality is the analysis' job.
  const LatticeElement &get(const Key &key) const {
    auto ptr = data.find(key);
    assert(ptr);
    return *ptr;
  }

  bool contains(const Key &key) const { return data.find(key) != nullptr; }

  auto begin() const { return data.begin(); }
  auto end() const { return data.end(); }

  // Structural sharing comparison (O(1) root-pointer check, falling back to
  // O(N))
  bool operator==(const ImmutableDataMap &other) const {
    return data == other.data;
  }

  // Lattice join: returns a merged map of this and other
  [[nodiscard]] ImmutableDataMap joinWith(const ImmutableDataMap &other) const {
    if (data == other.data) {
      return *this;
    }
    auto result = data.transient();
    for (const auto &[key, val] : other.data) {
      if (auto existing = result.find(key)) {
        result.set(key, existing->joinWith(val));
      } else {
        result.set(key, val);
      }
    }
    return ImmutableDataMap(result.persistent());
  }
};

// ============================================================================
// 2. TransferFunction Interface
// ============================================================================
// Encapsulates the analysis-specific logic for processing statements.
template <typename State> class TransferFunction {
public:
  virtual ~TransferFunction() = default;

  // Process a single statement. Takes the statement and its incoming merged
  // state.
  virtual State transferStatement(IRIContext &ctx, IRID stmt,
                                  const State &incomingState) = 0;

  // Optional edge transfer logic (e.g., conditional jump refinements)
  virtual State transferEdge(BBIDX from, BBIDX to, const State &exitState) {
    return exitState;
  }
};

// ============================================================================
// 3. Worklist Dataflow Solver
// ============================================================================
// A generic solver to compute fixed-point abstract interpretation on the CFG.
template <typename State> class DataflowSolver {
private:
  const IRICFG &cfg;
  TransferFunction<State> &transferFn;
  std::vector<State> blockEntryStates;
  std::vector<std::vector<State>> stmtStates;

public:
  DataflowSolver(const IRICFG &cfg, TransferFunction<State> &tf)
      : cfg(cfg), transferFn(tf) {}

  void run(const State &BI, const State &INIT, EdgeMask mask = EDGE_ALL) {
    // 1. Get Reverse Post-Order for fastest fixed-point convergence
    std::vector<BBIDX> rpo = cfg.reversePostOrder(mask);

    // 2. One slot per block at INIT, then the boundary state on top.
    blockEntryStates.assign(cfg.numBBs(), INIT);
    stmtStates.resize(cfg.numBBs());
    blockEntryStates[cfg.entry] = BI;

    std::queue<BBIDX> worklist;
    std::vector<char> inWorklist(cfg.numBBs(), 0);

    // Seed the worklist with blocks in RPO order
    for (BBIDX block : rpo) {
      worklist.push(block);
      inWorklist[block] = 1;
    }

    // Helper to propagate state to a target block and queue it if updated
    auto propagateState = [&](BBIDX target, const State &state) {
      State &oldEntry = blockEntryStates[target];
      State joined = oldEntry.joinWith(state);
      if (!(joined == oldEntry)) {
        oldEntry = std::move(joined);
        if (!inWorklist[target]) {
          worklist.push(target);
          inWorklist[target] = 1;
        }
      }
    };

    // 3. Iteration loop
    while (!worklist.empty()) {
      BBIDX currIdx = worklist.front();
      worklist.pop();
      inWorklist[currIdx] = 0;

      const IRIBB &bb = cfg.at(currIdx);
      State currState = blockEntryStates[currIdx];
      BBIDX exceptionTarget = cfg.exceptionOf(currIdx);

      bool exceptionPropagatedForCurrentState = false;

      // Helper to propagate state to the exception handler
      auto handleException = [&](const State &stateBefore) {
        if (exceptionTarget != NO_BB && !exceptionPropagatedForCurrentState) {
          propagateState(exceptionTarget, stateBefore);
          exceptionPropagatedForCurrentState = true;
        }
      };

      // Helper to transfer a statement
      auto runTransfer = [&](IRID stmtID) {
        State nextState =
            transferFn.transferStatement(cfg.ctx, stmtID, currState);
        if (!(nextState == currState)) {
          currState = std::move(nextState);
          exceptionPropagatedForCurrentState = false;
        }
      };

      // Record the state at each boundary as the walk passes it.
      std::vector<State> boundaries;
      boundaries.reserve(bb.stmts.size() + 1);

      // Transfer through non-terminal statements
      for (IRID stmtID : bb.stmts) {
        boundaries.push_back(currState);
        handleException(currState);
        runTransfer(stmtID);
      }
      boundaries.push_back(currState);
      stmtStates[currIdx] = std::move(boundaries);

      // Transfer through terminal statement
      if (bb.terminal != NO_IRID) {
        handleException(currState);
        runTransfer(bb.terminal);
      }

      // new CFG edges
      for (Edge e : bb.succs)
        if (EDGE_FLOW & edgeMask(e.kind)) {
          State edgeState = transferFn.transferEdge(currIdx, e.to, currState);
          propagateState(e.to, edgeState);
        }
    }
  }

  const State &getBlockEntryState(BBIDX block) const {
    return blockEntryStates.at(block);
  }

  const State &getStateBeforeStatement(BBIDX block, size_t index) const {
    return stmtStates.at(block).at(index);
  }

};

// ============================================================================
// 3.1. Yet Another Dataflow Solver
// ============================================================================
// A generic solver to compute fixed-point abstract interpretation on the CFG
// with the added benefit of picking the starting BB (needed when working with
// closures that can be continued later). Also returns a list of continuations
//
// WIP: will be deprecated soon
#if 0

template <typename State> class ResumableDataflowState {
public:
  IRICFG *cfg;
  std::unordered_map<BBIDX, State> retainedState;
  BBIDX startBBIDX;
  std::set<BBIDX> externalStateEntrypoints;

  ResumableDataflowState(IRICFG *cfg, BBIDX startBBIDX, State startState) {
    this->cfg = cfg;
    this->startBBIDX = startBBIDX;
    this->retainedState[startBBIDX] = startState;
  }

  void initializeSolverState(std::unordered_map<BBIDX, State> &state) {
    for (auto &e : retainedState) {
      state[e.first] = e.second;
    }
  }

  void addExternalStateEntrypoint(BBIDX bbIDX, State startState) {
    externalStateEntrypoints.insert(bbIDX);
    retainedState[bbIDX] = startState;
  }
};

template <typename State> class YADataflowSolver {
private:
  TransferFunction<State> &transferFn;
  std::unordered_map<BBIDX, State> blockEntryStates;

public:
  YADataflowSolver(TransferFunction<State> &tf) : transferFn(tf) {}

  // Run the worklist solver to fixed-point convergence
  std::unordered_map<IRIStatement *, State>
  run(ResumableDataflowState<State> resumptionState,
      bool includeExceptions = false) {
    IRICFG *cfg = resumptionState.cfg;

    auto cs =
        cfg->ctx.debugger.traceWriter->enterClosure(cfg->getDebugID(), "", "entry");

    // Results
    std::unordered_map<IRIStatement *, State> cfgAtStmt;
    // std::vector<ResumableDataflowState<State>> suspendedEvaluations;
    // std::unordered_map<BBIDX, State> suspendedStates;

    // Get Reverse Post-Order for fastest fixed-point convergence
    std::vector<BBIDX> rpo =
        cfg->getReversePostOrder(includeExceptions, resumptionState.startBBIDX);

    // Initialize block entry states to State::bottom()
    for (BBIDX block : rpo) {
      blockEntryStates[block] = State::bottom();
    }

    // Set Initial State (continuations may set more than oen).
    resumptionState.initializeSolverState(blockEntryStates);

    std::queue<BBIDX> worklist;
    std::unordered_set<BBIDX> inWorklist;

    // Seed the worklist with blocks in RPO order
    for (BBIDX block : rpo) {
      worklist.push(block);
      inWorklist.insert(block);
    }

    // Helper to propagate state to a target block and queue it if updated
    auto propagateState = [&](BBIDX target, const State &state) {
      State &oldEntry = blockEntryStates[target];
      State joined = oldEntry.joinWith(state);
      if (!(joined == oldEntry)) {
        oldEntry = std::move(joined);
        if (!inWorklist.contains(target)) {
          worklist.push(target);
          inWorklist.insert(target);
        }
      }
    };

    size_t iter = 0;
    // 3. Iteration loop
    while (!worklist.empty()) {
      BBIDX currIdx = worklist.front();
      worklist.pop();
      inWorklist.erase(currIdx);

      IRIBB *bb = cfg->get_bb(currIdx);
      State currState = blockEntryStates[currIdx];
      BBIDX exceptionTarget = bb->EXCEPTION;

      auto bs = cfg->ctx.debugger.traceWriter->enterBlock(bb->getDebugID(), iter++);

      bool exceptionPropagatedForCurrentState = false;
      auto handleException = [&](const State &stateBefore) {
        if (exceptionTarget != NO_BB && !exceptionPropagatedForCurrentState) {
          propagateState(exceptionTarget, stateBefore);
          exceptionPropagatedForCurrentState = true;
        }
      };

      // Helper to transfer a statement
      auto runTransfer = [&](IRIStatement *s) {
        auto e = cfg->ctx.debugger.traceWriter->eval(s->getDebugID());

        State nextState = transferFn.transferStatement(*s, currState);

        std::stringstream ss;
        currState.dumpDOT(ss, "YADataflowSolver",
                          cfg->ctx.debugger.traceNodeMetaMapper);
        e.state(ss.str());

        bool changed = nextState != currState;
        e.changed(changed);

        if (changed) {
          currState = std::move(nextState);
          exceptionPropagatedForCurrentState = false;
        }
      };

      // Transfer through non-terminal statements
      for (IRIStatement *s = bb->head; s != nullptr; s = s->next) {
        handleException(currState);
        runTransfer(s);
        cfgAtStmt[s] = currState;
      }

      bool isAwaitSuspension = false;

      // Transfer through terminal statement
      if (bb->tail) {
        handleException(currState);
        runTransfer(bb->tail);
        cfgAtStmt[bb->tail] = currState;
        if (IRI_NODE(cfg->ctx, bb->tail->id).tag == IRI_GEN::Goto) {
          GotoSEXP gt(bb->tail->id, cfg->ctx);
          if (gt.hasDeferred()) {
            throw std::runtime_error("WIP Await Semantics");
            // BBIDX targetIDX = gt.getIDX();
            //
            // State existingSuspended = suspendedStates.contains(targetIDX)
            //                               ? suspendedStates[targetIDX]
            //                               : State::bottom();
            // suspendedStates[targetIDX] =
            // existingSuspended.joinWith(currState);
            //
            // // Assert that this BB is a known continuation target in the CFG
            // if (!cfg->continuationPoints.contains(targetIDX)) {
            //   throw std::runtime_error("Expected Deferred Computation Target"
            //                            "to be a Continue Point in CFG -> " +
            //                            std::to_string(targetIDX));
            // }
            // isAwaitSuspension = true;
          }
        }
      }

      // Propagate exit state to normal successors
      if (!isAwaitSuspension) {
        for (BBIDX succ : cfg->successorsOf(currIdx)) {
          State edgeState = transferFn.transferEdge(currIdx, succ, currState);
          propagateState(succ, edgeState);
        }
      }
    }

    // // Prepare resumption points
    // for (auto &e : suspendedStates) {
    //   // The entry state is union of (i) combined synchronous state (ii)
    //   // combined asynchronous state
    //   auto &sIDX = e.first;
    //   auto suspensionState = e.second.joinWith(getBlockEntryState(sIDX));
    //   ResumableDataflowState<State> r(cfg, sIDX, suspensionState);
    //
    //   auto rBBs = cfg->getReversePostOrder(true, sIDX);
    //   std::set<BBIDX> reachableSet(rBBs.begin(), rBBs.end());
    //   for (auto &rIDX : reachableSet) {
    //     if (rIDX == sIDX)
    //       continue; // Ignore the entrypoint
    //     assert(cfg->predecessors.contains(rIDX));
    //     for (auto &predIDX : cfg->predecessors.at(rIDX)) {
    //       if (!reachableSet.contains(predIDX)) {
    //         State s =
    //             suspendedStates.contains(rIDX)
    //                 ?
    //                 suspendedStates[rIDX].joinWith(getBlockEntryState(rIDX))
    //                 : getBlockEntryState(rIDX);
    //         r.addExternalStateEntrypoint(rIDX, s);
    //         break;
    //       }
    //     }
    //   }
    //   suspendedEvaluations.push_back(r);
    // }
    return cfgAtStmt;
  }

  // Returns the entry state for a given basic block
  const State &getBlockEntryState(BBIDX block) const {
    auto it = blockEntryStates.find(block);
    if (it != blockEntryStates.end()) {
      return it->second;
    }
    static const State bottomState = State::bottom();
    return bottomState;
  }

  // On-demand query: Computes and returns the state immediately before a
  // statement is executed
  State queryStateAtStatement(const IRIStatement &stmt) {
    BBIDX blockIdx = stmt.bb->IDX;
    State state = getBlockEntryState(blockIdx);

    const auto &bb = stmt.bb;
    for (IRIStatement *s = bb->head; s != nullptr && s != &stmt; s = s->next) {
      state = transferFn.transferStatement(*s, state);
    }
    return state;
  }
};

#endif

// ============================================================================
// 4. Backward Worklist Dataflow Solver
// ============================================================================
// A generic solver to compute backward fixed-point dataflow analysis on the
// CFG.
template <typename State> class BackwardDataflowSolver {
private:
  const IRICFG &cfg;
  TransferFunction<State> &transferFn;
  std::vector<State> blockEntryStates;
  std::vector<State> blockExitStates;
  std::vector<std::vector<State>> stmtStates;

public:
  BackwardDataflowSolver(const IRICFG &cfg, TransferFunction<State> &tf)
      : cfg(cfg), transferFn(tf) {}

  void run(const State &BI, const State &INIT, EdgeMask mask = EDGE_ALL) {
    // 1. Post-Order, so a block is run after all the blocks it flows to.
    std::vector<BBIDX> po = cfg.reversePostOrder(mask);
    std::reverse(po.begin(), po.end());

    // 2. One slot per block at INIT, then the boundary state on top.
    blockEntryStates.assign(cfg.numBBs(), INIT);
    blockExitStates.assign(cfg.numBBs(), INIT);
    stmtStates.resize(cfg.numBBs());
    blockExitStates[cfg.exit] = BI;

    std::queue<BBIDX> worklist;
    std::vector<char> inWorklist(cfg.numBBs(), 0);

    // Seed the worklist with blocks in Post-Order
    for (BBIDX block : po) {
      worklist.push(block);
      inWorklist[block] = 1;
    }

    // Helper to propagate state to a predecessor block and queue it if updated
    auto propagateState = [&](BBIDX target, const State &state) {
      State &oldExit = blockExitStates[target];
      State joined = oldExit.joinWith(state);
      if (!(joined == oldExit)) {
        oldExit = std::move(joined);
        if (!inWorklist[target]) {
          worklist.push(target);
          inWorklist[target] = 1;
        }
      }
    };

    // 3. Iteration loop
    while (!worklist.empty()) {
      BBIDX currIdx = worklist.front();
      worklist.pop();
      inWorklist[currIdx] = 0;

      const IRIBB &bb = cfg.at(currIdx);
      State oldEntryState = blockEntryStates[currIdx];
      State currState = blockExitStates[currIdx];
      BBIDX exceptionTarget = cfg.exceptionOf(currIdx);

      // A throw can leave the block from any statement, so the handler's entry
      // state joins in at the statement, not at the block boundary. It only has
      // to be merged again once the state itself has moved.
      bool exceptionJoinedForCurrentState = false;
      auto handleException = [&]() {
        if ((mask & EDGE_EXCEPTION) && exceptionTarget != NO_BB &&
            !exceptionJoinedForCurrentState) {
          currState = currState.joinWith(blockEntryStates[exceptionTarget]);
          exceptionJoinedForCurrentState = true;
        }
      };

      // Helper to transfer a statement
      auto runTransfer = [&](IRID stmtID) {
        State nextState =
            transferFn.transferStatement(cfg.ctx, stmtID, currState);
        if (!(nextState == currState)) {
          currState = std::move(nextState);
          exceptionJoinedForCurrentState = false;
        }
      };

      // Transfer backward through the terminal statement
      if (bb.terminal != NO_IRID) {
        runTransfer(bb.terminal);
        handleException();
      }

      // currState is the state after stmts[i] just before stmts[i] is
      // transferred, which is the boundary worth recording.
      std::vector<State> after(bb.stmts.size());
      for (size_t i = bb.stmts.size(); i-- > 0;) {
        after[i] = currState;
        runTransfer(bb.stmts[i]);
        handleException();
      }
      stmtStates[currIdx] = std::move(after);

      blockEntryStates[currIdx] = currState;

      // If the entry state of this block changed, propagate changes to
      // predecessors
      if (currState == oldEntryState)
        continue;

      for (Edge e : bb.preds) {
        if (!(mask & edgeMask(e.kind)))
          continue;
        if (e.kind == EdgeKind::Exception) {
          // The state along a throw path was already merged by handleException
          // above; the exceptional predecessor only needs to be re-run so that
          // merge is seen.
          if (!inWorklist[e.to]) {
            worklist.push(e.to);
            inWorklist[e.to] = 1;
          }
          continue;
        }
        State edgeState = transferFn.transferEdge(currIdx, e.to, currState);
        propagateState(e.to, edgeState);
      }
    }
  }

  // Returns the entry state (state before the first statement) for a block
  const State &getBlockEntryState(BBIDX block) const {
    return blockEntryStates.at(block);
  }

  // Returns the exit state (state after the last statement) for a block
  const State &getBlockExitState(BBIDX block) const {
    return blockExitStates.at(block);
  }

  // Returns the state after stmts[index], which for a backward analysis is the
  // boundary the statement itself is judged against.
  const State &getStateAfterStatement(BBIDX block, size_t index) const {
    return stmtStates.at(block).at(index);
  }
};

} // namespace IRI_STRUCTURAL
