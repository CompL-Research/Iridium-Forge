#pragma once

#include "Support/IRICFG.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <typeindex>
#include <stdexcept>

namespace IRI_STRUCTURAL {

class AnalysisManager;

// ============================================================================
// 1. Pass Manager Base Concepts
// ============================================================================

// Type-erased wrapper for analysis results
struct AnalysisResultConcept {
  virtual ~AnalysisResultConcept() = default;
};

template <typename ResultT>
struct AnalysisResultModel : public AnalysisResultConcept {
  ResultT result;
  explicit AnalysisResultModel(ResultT r) : result(std::move(r)) {}
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
