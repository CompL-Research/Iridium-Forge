#pragma once

#include "IRICFG.hpp"
#include <immer/map.hpp>
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
    auto result = data;
    for (const auto& [key, val] : other.data) {
      if (auto existing = result.find(key)) {
        LatticeElement merged = *existing;
        merged.joinWith(val); // In-place join on the lattice element
        result = result.set(key, std::move(merged));
      } else {
        result = result.set(key, val);
      }
    }
    return ImmutableDataMap(std::move(result));
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

      // Sound exceptional control flow propagation before each statement is executed
      auto handleException = [&](const State& stateBefore) {
        if (exceptionTarget > -1) {
          propagateState(exceptionTarget, stateBefore);
        }
      };

      // Transfer through non-terminal statements
      for (IRIStatement* s = bb->head; s != nullptr; s = s->next) {
        handleException(currState);
        currState = transferFn.transferStatement(*s, currState);
      }

      // Transfer through terminal statement
      if (bb->tail) {
        handleException(currState);
        currState = transferFn.transferStatement(*bb->tail, currState);
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

} // namespace IRI_STRUCTURAL
