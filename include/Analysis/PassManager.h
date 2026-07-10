#pragma once

#include "Support/IRICFG.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <typeindex>
#include <stdexcept>
#include <type_traits>
#include <sstream>

namespace IRI_STRUCTURAL {

class AnalysisManager;

// ============================================================================
// 1. Dataflow Result Concept
// ============================================================================
// Generic interface that dataflow analysis results must implement to support
// unified debugging, visualization, and dumping.
struct DataflowResultConcept {
  virtual ~DataflowResultConcept() = default;
  virtual std::string getAnalysisName() const = 0;
  virtual void dumpStateAtStatement(const IRIStatement& stmt, IRI_STORAGE::IridiumPool& pool, std::ostream& os) const = 0;
  virtual void dumpBlockEntryState(BBIDX block, IRI_STORAGE::IridiumPool& pool, std::ostream& os) const = 0;
  virtual void dumpBlockExitState(BBIDX block, IRI_STORAGE::IridiumPool& pool, std::ostream& os) const = 0;
};

// ============================================================================
// 2. Pass Manager Base Concepts
// ============================================================================

// Type-erased wrapper for analysis results
struct AnalysisResultConcept {
  virtual ~AnalysisResultConcept() = default;
  virtual const DataflowResultConcept* asDataflow() const { return nullptr; }
};

template <typename ResultT>
struct AnalysisResultModel : public AnalysisResultConcept {
  ResultT result;
  explicit AnalysisResultModel(ResultT r) : result(std::move(r)) {}

  const DataflowResultConcept* asDataflow() const override {
    if constexpr (std::is_base_of_v<DataflowResultConcept, ResultT>) {
      return &result;
    }
    return nullptr;
  }
};

// ============================================================================
// 2. Analysis Manager
// ============================================================================
// Orchestrates analysis passes. Lazily computes and caches analysis results.
class AnalysisManager {
private:
  // Map of static pass ID pointers to cached type-erased results
  std::unordered_map<const void*, std::unique_ptr<AnalysisResultConcept>> cache;

public:
  AnalysisManager() = default;
  ~AnalysisManager() = default;

  // Query an analysis. If it is already cached, returns it immediately.
  // Otherwise, runs it, caches the result, and returns it.
  template <typename AnalysisPass>
  const typename AnalysisPass::Result& getResult(IRICFG& cfg) {
    const void* passID = &AnalysisPass::ID;
    auto it = cache.find(passID);
    if (it == cache.end()) {
      // Lazy computation: instantiate the pass and run it
      AnalysisPass pass;
      typename AnalysisPass::Result res = pass.run(cfg, *this);
      
      auto model = std::make_unique<AnalysisResultModel<typename AnalysisPass::Result>>(std::move(res));
      it = cache.emplace(passID, std::move(model)).first;
    }
    return static_cast<AnalysisResultModel<typename AnalysisPass::Result>*>(it->second.get())->result;
  }

  // Manually invalidate a specific analysis result (e.g. if a transformation affects only it)
  template <typename AnalysisPass>
  void invalidate() {
    cache.erase(&AnalysisPass::ID);
  }

  // Invalidate all cached analysis results (usually called after any IR transformation)
  void invalidateAll() {
    cache.clear();
  }

  // Dump all cached dataflow analyses statement-by-statement for debugging
  void dumpDataflowStates(IRICFG& cfg, std::ostream& os) {
    for (const auto& [idx, bb] : cfg.nodeMap) {
      os << "  BB" << idx << ":\n";

      // 1. Block Entry States
      for (const auto& [passID, model] : cache) {
        if (auto df = model->asDataflow()) {
          os << "    [BlockEntry (" << df->getAnalysisName() << "): ";
          df->dumpBlockEntryState(idx, cfg.pool, os);
          os << "]\n";
        }
      }

      // 2. Statements
      IRIStatement* curr = bb->head;
      while (curr != nullptr) {
        for (const auto& [passID, model] : cache) {
          if (auto df = model->asDataflow()) {
            os << "    [State (" << df->getAnalysisName() << "): ";
            df->dumpStateAtStatement(*curr, cfg.pool, os);
            os << "]\n";
          }
        }

        std::stringstream ss;
        cfg.pool[curr->id].dumpFlat(ss, &cfg.pool, 0, false);
        std::string rawStr = ss.str();
        std::stringstream statementLines(rawStr);
        std::string line;
        while (std::getline(statementLines, line)) {
          os << "      " << line << "\n";
        }
        os << "\n";
        curr = curr->next;
      }

      // 3. Terminal Statement BlockExit States
      if (bb->tail != nullptr) {
        for (const auto& [passID, model] : cache) {
          if (auto df = model->asDataflow()) {
            os << "    [State (" << df->getAnalysisName() << "): ";
            df->dumpStateAtStatement(*(bb->tail), cfg.pool, os);
            os << "]\n";
          }
        }
        std::stringstream ss;
        cfg.pool[bb->tail->id].dumpFlat(ss, &cfg.pool, 0, false);
        std::string rawStr = ss.str();
        std::stringstream statementLines(rawStr);
        std::string line;
        while (std::getline(statementLines, line)) {
          os << "      " << line << "\n";
        }
        os << "\n";
      }
    }
  }
};

// ============================================================================
// 3. Transformation Pass Manager
// ============================================================================
// Manages a pipeline of transformation passes.
class PassManager {
private:
  struct PassConcept {
    virtual ~PassConcept() = default;
    virtual void run(IRICFG& cfg, AnalysisManager& am) = 0;
  };

  template <typename PassT>
  struct PassModel : public PassConcept {
    PassT pass;
    explicit PassModel(PassT p) : pass(std::move(p)) {}
    
    void run(IRICFG& cfg, AnalysisManager& am) override {
      pass.run(cfg, am);
    }
  };

  std::vector<std::unique_ptr<PassConcept>> passes;

public:
  PassManager() = default;
  ~PassManager() = default;

  // Add a pass to the pipeline
  template <typename PassT>
  void addPass(PassT pass) {
    passes.push_back(std::make_unique<PassModel<PassT>>(std::move(pass)));
  }

  // Run all passes in the pipeline sequentially over a CFG
  void run(IRICFG& cfg, AnalysisManager& am) {
    for (auto& pass : passes) {
      pass->run(cfg, am);
    }
  }

  // Helper to clear the pipeline
  void clear() {
    passes.clear();
  }
};

} // namespace IRI_STRUCTURAL
