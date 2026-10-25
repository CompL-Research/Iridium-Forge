#pragma once

#include "Support/IRICFG.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <typeindex>
#include <stdexcept>
#include <type_traits>
#include <sstream>
#include <chrono>
#include <cstdlib>
#include <typeinfo>
#include <iostream>

#if defined(__GNUC__) || defined(__clang__)
#include <cxxabi.h>
inline std::string demangle(const char* name) {
  int status = -1;
  std::unique_ptr<char, void(*)(void*)> res {
    abi::__cxa_demangle(name, NULL, NULL, &status),
    std::free
  };
  return (status == 0) ? res.get() : name;
}
#else
inline std::string demangle(const char* name) { return name; }
#endif

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
  virtual void dumpStateAtStatement(const IRIStatement& stmt, IRI_STORAGE::IRIContext& ctx, std::ostream& os) const = 0;
  virtual void dumpBlockEntryState(BBIDX block, IRI_STORAGE::IRIContext& ctx, std::ostream& os) const = 0;
  virtual void dumpBlockExitState(BBIDX block, IRI_STORAGE::IRIContext& ctx, std::ostream& os) const = 0;
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
  bool enableLogging = false;
  std::unordered_map<std::string, double> analysisPassDurations;

public:
  explicit AnalysisManager(bool log = false)
      : enableLogging(log || std::getenv("LOG_PASS_PERF") != nullptr) {}

  void setLogging(bool log) { enableLogging = log; }

  ~AnalysisManager() {
    if (enableLogging && !analysisPassDurations.empty()) {
      std::cout << "\n=== Analysis Pass Performance Summary ===\n";
      for (const auto& [name, duration] : analysisPassDurations) {
        std::cout << "  " << name << ": " << duration << " ms\n";
      }
      std::cout << "=========================================\n";
    }
  }

  // Query an analysis. If it is already cached, returns it immediately.
  // Otherwise, runs it, caches the result, and returns it.
  template <typename AnalysisPass>
  const typename AnalysisPass::Result& getResult(IRICFG& cfg) {
    const void* passID = &AnalysisPass::ID;
    auto it = cache.find(passID);
    if (it == cache.end()) {
      auto start = std::chrono::high_resolution_clock::now();

      // Lazy computation: instantiate the pass and run it
      AnalysisPass pass;
      typename AnalysisPass::Result res = pass.run(cfg, *this);

      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> elapsed = end - start;

      if (enableLogging) {
        std::string name = demangle(typeid(AnalysisPass).name());
        analysisPassDurations[name] += elapsed.count();
      }
      
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
          df->dumpBlockEntryState(idx, cfg.ctx, os);
          os << "]\n";
        }
      }

      // 2. Statements
      IRIStatement* curr = bb->head;
      while (curr != nullptr) {
        for (const auto& [passID, model] : cache) {
          if (auto df = model->asDataflow()) {
            os << "    [State (" << df->getAnalysisName() << "): ";
            df->dumpStateAtStatement(*curr, cfg.ctx, os);
            os << "]\n";
          }
        }

        std::stringstream ss;
        IRI_NODE(cfg.ctx, curr->id).dumpFlat(ss, &cfg.ctx, 0, false);
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
            df->dumpStateAtStatement(*(bb->tail), cfg.ctx, os);
            os << "]\n";
          }
        }
        std::stringstream ss;
        IRI_NODE(cfg.ctx, bb->tail->id).dumpFlat(ss, &cfg.ctx, 0, false);
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
    virtual bool run(IRICFG& cfg, AnalysisManager& am) = 0;
    virtual std::string getName() const = 0;
  };

  template <typename PassT>
  struct PassModel : public PassConcept {
    PassT pass;
    explicit PassModel(PassT p) : pass(std::move(p)) {}
    
    bool run(IRICFG& cfg, AnalysisManager& am) override {
      return pass.run(cfg, am);
    }

    std::string getName() const override {
      return demangle(typeid(PassT).name());
    }
  };

  std::vector<std::unique_ptr<PassConcept>> passes;
  bool enableLogging = false;
  std::unordered_map<std::string, double> optPassDurations;

public:
  explicit PassManager(bool log = false)
      : enableLogging(log || std::getenv("LOG_PASS_PERF") != nullptr) {}

  void setLogging(bool log) { enableLogging = log; }

  ~PassManager() {
    if (enableLogging && !optPassDurations.empty()) {
      std::cout << "\n=== Optimization Pass Performance Summary ===\n";
      for (const auto& [name, duration] : optPassDurations) {
        std::cout << "  " << name << ": " << duration << " ms\n";
      }
      std::cout << "=============================================\n";
    }
  }

  // Add a pass to the pipeline
  template <typename PassT>
  void addPass(PassT pass) {
    passes.push_back(std::make_unique<PassModel<PassT>>(std::move(pass)));
  }

  // Run all passes in the pipeline sequentially over a CFG until fixed-point
  void run(IRICFG& cfg, AnalysisManager& am) {
    bool changed = true;
    int iterations = 0;
    const int maxIterations = 10;
    while (changed && iterations < maxIterations) {
      changed = false;
      for (auto& pass : passes) {
        auto start = std::chrono::high_resolution_clock::now();
        bool passChanged = pass->run(cfg, am);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end - start;

        if (enableLogging) {
          optPassDurations[pass->getName()] += elapsed.count();
        }

        if (passChanged) {
          changed = true;
        }
      }
      iterations++;
    }
  }

  // Helper to clear the pipeline
  void clear() {
    passes.clear();
  }
};

} // namespace IRI_STRUCTURAL
