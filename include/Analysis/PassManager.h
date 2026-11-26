#pragma once

#include "Support/IRICFG.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#if defined(__GNUC__) || defined(__clang__)
#include <cxxabi.h>
inline std::string demangle(const char *name) {
  int status = -1;
  std::unique_ptr<char, void (*)(void *)> res{
      abi::__cxa_demangle(name, NULL, NULL, &status), std::free};
  return (status == 0) ? res.get() : name;
}
#else
inline std::string demangle(const char *name) { return name; }
#endif

namespace IRI_STRUCTURAL {

class AnalysisManager;

// ============================================================================
// 1. Dataflow Result Concept
// ============================================================================
struct DataflowResultConcept {
  virtual ~DataflowResultConcept() = default;
  virtual std::string getAnalysisName() const = 0;
  // virtual void dumpStateAtStatement(const IRICFG &cfg, BBIDX block, IRID stmt,
  //                                   std::ostream &os) const = 0;
  // virtual void dumpBlockEntryState(const IRICFG &cfg, BBIDX block,
  //                                  std::ostream &os) const = 0;
  // virtual void dumpBlockExitState(const IRICFG &cfg, BBIDX block,
  //                                 std::ostream &os) const = 0;
};

// ============================================================================
// 2. Pass Manager Base Concepts
// ============================================================================
struct AnalysisResultConcept {
  virtual ~AnalysisResultConcept() = default;
  virtual const DataflowResultConcept *asDataflow() const { return nullptr; }
};

template <typename ResultT>
struct AnalysisResultModel : public AnalysisResultConcept {
  ResultT result;
  explicit AnalysisResultModel(ResultT r) : result(std::move(r)) {}

  const DataflowResultConcept *asDataflow() const override {
    if constexpr (std::is_base_of_v<DataflowResultConcept, ResultT>) {
      return &result;
    }
    return nullptr;
  }
};

// ============================================================================
// 2. Analysis Manager
// ============================================================================
class AnalysisManager {
private:
  std::unordered_map<
      const IRICFG *,
      std::unordered_map<const void *, std::unique_ptr<AnalysisResultConcept>>>
      cache;
  bool enableLogging = false;
  std::unordered_map<std::string, double> analysisPassDurations;

public:
  explicit AnalysisManager(bool log = false)
      : enableLogging(log || std::getenv("LOG_PASS_PERF") != nullptr) {}

  void setLogging(bool log) { enableLogging = log; }

  ~AnalysisManager() {
    if (enableLogging && !analysisPassDurations.empty()) {
      std::cout << "\n=== Analysis Pass Performance Summary ===\n";
      for (const auto &[name, duration] : analysisPassDurations) {
        std::cout << "  " << name << ": " << duration << " ms\n";
      }
      std::cout << "=========================================\n";
    }
  }

  template <typename AnalysisPass>
  const typename AnalysisPass::Result &getResult(IRICFG &cfg) {
    const void *passID = &AnalysisPass::ID;
    auto &perCFG = cache[&cfg];
    auto it = perCFG.find(passID);
    if (it == perCFG.end()) {
      auto start = std::chrono::high_resolution_clock::now();

      AnalysisPass pass;
      typename AnalysisPass::Result res = pass.run(cfg, *this);

      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double, std::milli> elapsed = end - start;

      if (enableLogging) {
        std::string name = demangle(typeid(AnalysisPass).name());
        analysisPassDurations[name] += elapsed.count();
      }

      auto model =
          std::make_unique<AnalysisResultModel<typename AnalysisPass::Result>>(
              std::move(res));
      it = perCFG.emplace(passID, std::move(model)).first;
    }
    return static_cast<AnalysisResultModel<typename AnalysisPass::Result> *>(
               it->second.get())
        ->result;
  }

  void invalidate(const IRICFG &cfg) { cache.erase(&cfg); }

  // void dumpDataflowStates(IRICFG &cfg, std::ostream &os) {
  //   using Entries = std::unordered_map<const void *,
  //                                      std::unique_ptr<AnalysisResultConcept>>;
  //   static const Entries none;
  //   auto found = cache.find(&cfg);
  //   const Entries &cached = found == cache.end() ? none : found->second;
  //
  //   auto dumpStatement = [&](IRID stmtID) {
  //     std::stringstream ss;
  //     IRI_NODE(cfg.ctx, stmtID).dumpFlat(ss, &cfg.ctx, 0, false);
  //     std::stringstream statementLines(ss.str());
  //     std::string line;
  //     while (std::getline(statementLines, line)) {
  //       os << "      " << line << "\n";
  //     }
  //     os << "\n";
  //   };
  //
  //   for (BBIDX h = 0; h < cfg.numBBs(); h++) {
  //     const IRIBB &bb = cfg.at(h);
  //     os << "  BB" << h << ":\n";
  //
  //     // 1. Block Entry States
  //     for (const auto &[passID, model] : cached) {
  //       if (auto df = model->asDataflow()) {
  //         os << "    [BlockEntry (" << df->getAnalysisName() << "): ";
  //         df->dumpBlockEntryState(cfg, h, os);
  //         os << "]\n";
  //       }
  //     }
  //
  //     // 2. Statements
  //     for (IRID stmtID : bb.stmts) {
  //       for (const auto &[passID, model] : cached) {
  //         if (auto df = model->asDataflow()) {
  //           os << "    [State (" << df->getAnalysisName() << "): ";
  //           df->dumpStateAtStatement(cfg, h, stmtID, os);
  //           os << "]\n";
  //         }
  //       }
  //       dumpStatement(stmtID);
  //     }
  //
  //     // 3. Terminal Statement BlockExit States
  //     if (bb.terminal != NO_IRID) {
  //       for (const auto &[passID, model] : cached) {
  //         if (auto df = model->asDataflow()) {
  //           os << "    [State (" << df->getAnalysisName() << "): ";
  //           df->dumpStateAtStatement(cfg, h, bb.terminal, os);
  //           os << "]\n";
  //         }
  //       }
  //       dumpStatement(bb.terminal);
  //     }
  //   }
  // }
};

// ============================================================================
// 3. Transformation Pass Manager
// ============================================================================
class PassManager {
private:
  struct PassConcept {
    virtual ~PassConcept() = default;
    virtual bool run(IRICFG &cfg, AnalysisManager &am) = 0;
    virtual std::string getName() const = 0;
  };

  template <typename PassT> struct PassModel : public PassConcept {
    PassT pass;
    explicit PassModel(PassT p) : pass(std::move(p)) {}

    bool run(IRICFG &cfg, AnalysisManager &am) override {
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
  explicit PassManager(bool log = false) : enableLogging(log) {}

  void setLogging(bool log) { enableLogging = log; }

  ~PassManager() {
    if (enableLogging && !optPassDurations.empty()) {
      std::cout << "\n=== Optimization Pass Performance Summary ===\n";
      for (const auto &[name, duration] : optPassDurations) {
        std::cout << "  " << name << ": " << duration << " ms\n";
      }
      std::cout << "=============================================\n";
    }
  }

  // Add a pass to the pipeline
  template <typename PassT> void addPass(PassT pass) {
    passes.push_back(std::make_unique<PassModel<PassT>>(std::move(pass)));
  }

  // Run all passes in the pipeline sequentially over a CFG until fixed-point
  void run(IRICFG &cfg, AnalysisManager &am) {
    bool changed = true;
    int iterations = 0;
    const int maxIterations = 10;
    while (changed && iterations < maxIterations) {
      changed = false;
      for (auto &pass : passes) {
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
  void clear() { passes.clear(); }
};

} // namespace IRI_STRUCTURAL
