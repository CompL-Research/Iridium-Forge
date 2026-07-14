#pragma once

#include "IRICFG.hpp"
#include <immer/map.hpp>
#include <immer/map_transient.hpp>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <functional>
#include <iostream>
#include <algorithm>

namespace IRI_STRUCTURAL {

// ============================================================================
// 1. ImmutableDataMap
// ============================================================================
// A wrapper around immer::map to provide immutable value-semantics,
// structural sharing, and O(log N) updates for dataflow state maps.
template <typename Key, typename LatticeElement>
class ImmutableDataMap {
private:
  immer::map<Key, LatticeElement> data;

public:
  ImmutableDataMap() = default;
  explicit ImmutableDataMap(immer::map<Key, LatticeElement> d) : data(std::move(d)) {}

  // Returns a new map with the key set to the given lattice element (immutable update)
  ImmutableDataMap set(const Key& key, const LatticeElement& val) const {
    return ImmutableDataMap(data.set(key, val));
  }

  // Returns a new map with the key removed
  ImmutableDataMap erase(const Key& key) const {
    return ImmutableDataMap(data.erase(key));
  }

  // Returns the value for a key, or the defaultValue if not found
  const LatticeElement& get(const Key& key, const LatticeElement& defaultValue) const {
    if (auto ptr = data.find(key)) {
      return *ptr;
    }
    return defaultValue;
  }

  // Structural sharing comparison (O(1) root-pointer check, falling back to O(N))
  bool operator==(const ImmutableDataMap& other) const {
    return data == other.data;
  }

  // Lattice join: returns a merged map of this and other
  ImmutableDataMap joinWith(const ImmutableDataMap& other) const {
    if (data == other.data) {
      return *this;
    }
    auto result = data.transient();
    for (const auto& [key, val] : other.data) {
      if (auto existing = result.find(key)) {
        LatticeElement merged = *existing;
        merged.joinWith(val); // In-place join on the lattice element
        result.set(key, std::move(merged));
      } else {
        result.set(key, val);
      }
    }
    return ImmutableDataMap(result.persistent());
  }

  // Expose the underlying raw immer map if needed
  const immer::map<Key, LatticeElement>& getRawMap() const {
    return data;
  }
};

// ============================================================================
// 2. TransferFunction Interface
// ============================================================================
// Encapsulates the analysis-specific logic for processing statements.
template <typename State>
class TransferFunction {
public:
  virtual ~TransferFunction() = default;

  // Process a single statement. Takes the statement and its incoming merged state.
  virtual State transferStatement(const IRIStatement& stmt, const State& incomingState) = 0;

  // Optional edge transfer logic (e.g., conditional jump refinements)
  virtual State transferEdge(BBIDX from, BBIDX to, const State& exitState) {
    return exitState;
  }
};

// ============================================================================
// 3. Worklist Dataflow Solver
// ============================================================================
// A generic solver to compute fixed-point abstract interpretation on the CFG.
template <typename State>
class DataflowSolver {
private:
  const IRICFG& cfg;
  TransferFunction<State>& transferFn;
  std::unordered_map<BBIDX, State> blockEntryStates;

public:
  DataflowSolver(const IRICFG& cfg, TransferFunction<State>& tf)
      : cfg(cfg), transferFn(tf) {}

  // Run the worklist solver to fixed-point convergence
  void run(State entryState, bool includeExceptions = false) {
    // 1. Get Reverse Post-Order for fastest fixed-point convergence
    std::vector<BBIDX> rpo = cfg.getReversePostOrder(includeExceptions);

    // 2. Initialize block entry states to State::bottom()
    for (BBIDX block : rpo) {
      blockEntryStates[block] = State::bottom();
    }
    blockEntryStates[cfg.entry_block] = entryState;

    std::queue<BBIDX> worklist;
    std::unordered_set<BBIDX> inWorklist;

    // Seed the worklist with blocks in RPO order
    for (BBIDX block : rpo) {
      worklist.push(block);
      inWorklist.insert(block);
    }

    // Helper to propagate state to a target block and queue it if updated
    auto propagateState = [&](BBIDX target, const State& state) {
      State& oldEntry = blockEntryStates[target];
      State joined = oldEntry.joinWith(state);
      if (!(joined == oldEntry)) {
        oldEntry = std::move(joined);
        if (!inWorklist.contains(target)) {
          worklist.push(target);
          inWorklist.insert(target);
        }
      }
    };

    // 3. Iteration loop
    while (!worklist.empty()) {
      BBIDX currIdx = worklist.front();
      worklist.pop();
      inWorklist.erase(currIdx);

      const auto& bb = cfg.nodeMap.at(currIdx);
      State currState = blockEntryStates[currIdx];
      double exceptionTarget = bb->EXCEPTION;

      bool exceptionPropagatedForCurrentState = false;
      auto handleException = [&](const State& stateBefore) {
        if (exceptionTarget > -1 && !exceptionPropagatedForCurrentState) {
          propagateState(exceptionTarget, stateBefore);
          exceptionPropagatedForCurrentState = true;
        }
      };

      // Helper to transfer a statement
      auto runTransfer = [&](IRIStatement* s) {
        State nextState = transferFn.transferStatement(*s, currState);
        if (!(nextState == currState)) {
          currState = std::move(nextState);
          exceptionPropagatedForCurrentState = false;
        }
      };

      // Transfer through non-terminal statements
      for (IRIStatement* s = bb->head; s != nullptr; s = s->next) {
        handleException(currState);
        runTransfer(s);
      }

      // Transfer through terminal statement
      if (bb->tail) {
        handleException(currState);
        runTransfer(bb->tail);
      }

      // Propagate exit state to normal successors
      auto succIt = cfg.successors.find(currIdx);
      if (succIt != cfg.successors.end()) {
        for (BBIDX succ : succIt->second) {
          State edgeState = transferFn.transferEdge(currIdx, succ, currState);
          propagateState(succ, edgeState);
        }
      }
    }
  }

  // Returns the entry state for a given basic block
  const State& getBlockEntryState(BBIDX block) const {
    auto it = blockEntryStates.find(block);
    if (it != blockEntryStates.end()) {
      return it->second;
    }
    static const State bottomState = State::bottom();
    return bottomState;
  }

  // On-demand query: Computes and returns the state immediately before a statement is executed
  State queryStateAtStatement(const IRIStatement& stmt) {
    BBIDX blockIdx = stmt.bb->IDX;
    State state = getBlockEntryState(blockIdx);

    const auto& bb = stmt.bb;
    for (IRIStatement* s = bb->head; s != nullptr && s != &stmt; s = s->next) {
      state = transferFn.transferStatement(*s, state);
    }
    return state;
  }
};

// ============================================================================
// 4. Backward Worklist Dataflow Solver
// ============================================================================
// A generic solver to compute backward fixed-point dataflow analysis on the CFG.
template <typename State>
class BackwardDataflowSolver {
private:
  const IRICFG& cfg;
  TransferFunction<State>& transferFn;
  std::unordered_map<BBIDX, State> blockEntryStates;
  std::unordered_map<BBIDX, State> blockExitStates;

public:
  BackwardDataflowSolver(const IRICFG& cfg, TransferFunction<State>& tf)
      : cfg(cfg), transferFn(tf) {}

  // Run the worklist solver backward to fixed-point convergence
  void run(State exitState, bool includeExceptions = false) {
    // 1. Get Reverse Post-Order and reverse it to get Post-Order (for backward analysis)
    std::vector<BBIDX> rpo = cfg.getReversePostOrder(includeExceptions);
    std::vector<BBIDX> po = rpo;
    std::reverse(po.begin(), po.end());

    // 2. Initialize block states to State::bottom()
    for (BBIDX block : po) {
      blockEntryStates[block] = State::bottom();
      blockExitStates[block] = State::bottom();
    }

    // Build exceptional predecessors map
    std::unordered_map<BBIDX, std::vector<BBIDX>> exceptionalPredecessors;
    if (includeExceptions) {
      for (const auto& [idx, bb] : cfg.nodeMap) {
        if (bb->EXCEPTION > -1) {
          exceptionalPredecessors[bb->EXCEPTION].push_back(idx);
        }
      }
    }

    // Seed the exit state of exit blocks (blocks with no successors)
    for (BBIDX block : po) {
      if (cfg.successors.find(block) == cfg.successors.end() || cfg.successors.at(block).empty()) {
        blockExitStates[block] = exitState;
      }
    }

    std::queue<BBIDX> worklist;
    std::unordered_set<BBIDX> inWorklist;

    // Seed the worklist with all blocks in Post-Order
    for (BBIDX block : po) {
      worklist.push(block);
      inWorklist.insert(block);
    }

    // Helper to propagate state to a predecessor block (backward propagation)
    auto propagateState = [&](BBIDX target, const State& state) {
      State& oldExit = blockExitStates[target];
      State joined = oldExit.joinWith(state);
      if (!(joined == oldExit)) {
        oldExit = std::move(joined);
        if (!inWorklist.contains(target)) {
          worklist.push(target);
          inWorklist.insert(target);
        }
      }
    };

    // 3. Iteration loop
    while (!worklist.empty()) {
      BBIDX currIdx = worklist.front();
      worklist.pop();
      inWorklist.erase(currIdx);

      const auto& bb = cfg.nodeMap.at(currIdx);
      State oldEntryState = blockEntryStates[currIdx];
      State currState = blockExitStates[currIdx];

      // Sound exceptional control flow propagation
      bool exceptionJoinedForCurrentState = false;
      auto handleException = [&](State& stateBefore) {
        if (includeExceptions && bb->EXCEPTION > -1 && !exceptionJoinedForCurrentState) {
          stateBefore = stateBefore.joinWith(getBlockEntryState(bb->EXCEPTION));
          exceptionJoinedForCurrentState = true;
        }
      };

      // Transfer backward through terminal statement
      if (bb->tail) {
        State nextState = transferFn.transferStatement(*bb->tail, currState);
        if (!(nextState == currState)) {
          currState = std::move(nextState);
          exceptionJoinedForCurrentState = false;
        }
        handleException(currState);
      }

      // Transfer backward through non-terminal statements in reverse order (tail to head)
      std::vector<IRIStatement*> stmts;
      for (IRIStatement* s = bb->head; s != nullptr; s = s->next) {
        stmts.push_back(s);
      }
      for (auto it = stmts.rbegin(); it != stmts.rend(); ++it) {
        State nextState = transferFn.transferStatement(**it, currState);
        if (!(nextState == currState)) {
          currState = std::move(nextState);
          exceptionJoinedForCurrentState = false;
        }
        handleException(currState);
      }

      blockEntryStates[currIdx] = currState;

      // If the entry state of this block changed, propagate changes to predecessors
      if (!(currState == oldEntryState)) {
        // 1. Normal predecessors
        auto predIt = cfg.predecessors.find(currIdx);
        if (predIt != cfg.predecessors.end()) {
          for (BBIDX pred : predIt->second) {
            State edgeState = transferFn.transferEdge(currIdx, pred, currState);
            propagateState(pred, edgeState);
          }
        }

        // 2. Exceptional predecessors
        if (includeExceptions) {
          auto expIt = exceptionalPredecessors.find(currIdx);
          if (expIt != exceptionalPredecessors.end()) {
            for (BBIDX pred : expIt->second) {
              if (!inWorklist.contains(pred)) {
                worklist.push(pred);
                inWorklist.insert(pred);
              }
            }
          }
        }
      }
    }
  }

  // Returns the entry state (state before first statement) for a block
  const State& getBlockEntryState(BBIDX block) const {
    auto it = blockEntryStates.find(block);
    if (it != blockEntryStates.end()) {
      return it->second;
    }
    static const State bottomState = State::bottom();
    return bottomState;
  }

  // Returns the exit state (state after last statement) for a block
  const State& getBlockExitState(BBIDX block) const {
    auto it = blockExitStates.find(block);
    if (it != blockExitStates.end()) {
      return it->second;
    }
    static const State bottomState = State::bottom();
    return bottomState;
  }
};

} // namespace IRI_STRUCTURAL
