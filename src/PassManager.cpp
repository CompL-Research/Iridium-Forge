#include "Iridium/PassManager.h"

#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/Domains/EffectAtStmt.h"
#include "Iridium/Analysis/Domains/CopyPropInfo.h"
#include "Iridium/Analysis/Domains/SetSafePropKeyAccesses.h"
#include "Iridium/Analysis/DataflowSolver.h"
#include "Iridium/OptimizationPasses/ConstantPropPass.h"
#include "Iridium/OptimizationPasses/WriteBarrierReductionPass.h"
#include "Iridium/OptimizationPasses/CopyPropPass.h"
#include "Iridium/OptimizationPasses/PropEffectsPass.h"
#include "Iridium/OptimizationPasses/DCEPass.h"
#include "Iridium/OptimizationPasses/DeadBindingRemoval.h"
#include "Iridium/OptimizationPasses/ReduceComputedFieldOpsPass.h"
#include "Iridium/OptimizationPasses/RemoveRedundantPropKeyCastPass.h"
#include "Iridium/CorePasses/3_filterNops.h"
#include "external/json.hpp"
#include <fstream>

#include <filesystem>

#include <string>
#include <random>

#include <chrono>
#include <iostream>

#include <iostream>
#include <cstdlib>
#include <string>

void PassManager::buildPipeline(int level)
{
  pipeline.clear();

  if (!getenv("NO_CONSTPROP"))
    pipeline.push_back(std::make_unique<ConstantPropPass>());

  if (!getenv("NO_COPYPROP"))
    pipeline.push_back(std::make_unique<CopyPropPass>());

  if (!getenv("NO_WBR"))
    pipeline.push_back(std::make_unique<WriteBarrierReductionPass>());

  if (!getenv("NO_DCE"))
    pipeline.push_back(std::make_unique<DCEPass>());

  if (!getenv("NO_EPROP"))
    pipeline.push_back(std::make_unique<PropEffectsPass>());

  if (!getenv("NO_RKEYCAST"))
    pipeline.push_back(std::make_unique<RemoveRedundantPropKeyCastPass>());

  if (!getenv("NO_REDKEYCAST"))
    pipeline.push_back(std::make_unique<ReduceComputedFieldOpsPass>());
}

bool PassManager::isTainted(BBContainerView &bb, const std::set<double> &taintedScopes)
{
  for (auto &ts : taintedScopes)
  {
    if (isScopeReachable(ts,
                         bb.getScopeIdx(),
                         iridiumBuildContext))
    {
      bb.tainted = true;
      return true;
    }
  }
  return false;
}

void printOptimizationStatus()
{
  struct Flag
  {
    const char *env;  // NO_ flag
    const char *name; // positive feature name
  };

  Flag flags[] = {
      {"NO_CONSTPROP", "CONSTPROP"},
      {"NO_COPYPROP", "COPYPROP"},
      {"NO_WBR", "WBR"},
      {"NO_DCE", "DCE"},
      {"NO_EPROP", "EPROP"},
      {"NO_RKEYCAST", "RKEYCAST"},
      {"NO_REDKEYCAST", "REDKEYCAST"},
      {"NO_DEADBR", "DEADBR"}};

  for (const auto &f : flags)
  {
    bool disabled = (std::getenv(f.env) != nullptr);
    std::cerr << f.name << ": " << (disabled ? "OFF" : "ON") << "\n";
  }
}

#define INLINING_DEPTH 1

class ScopeTimer
{
public:
  explicit ScopeTimer(const std::string &name = "")
      : name_(name),
        start_(std::chrono::high_resolution_clock::now()) {}

  ~ScopeTimer()
  {
    using namespace std::chrono;
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start_).count();

    std::cerr << "[TIMER] " << (name_.empty() ? "Scope" : name_)
              << " took " << duration / 1000.0 << " ms\n";
  }

private:
  std::string name_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

std::string randomString(size_t length)
{
  static const std::string chars =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789";

  thread_local static std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<size_t> dist(0, chars.size() - 1);

  std::string result;
  result.reserve(length);
  for (size_t i = 0; i < length; i++)
  {
    result.push_back(chars[dist(rng)]);
  }
  return result;
}

using json = nlohmann::json;

void PassManager::justAnalysis(std::stringstream &ss, std::set<double> taintedScopes)
{
  size_t envReadRemoteTotal = 0, envReadRemoteSafe = 0;
  size_t envWriteRemoteTotal = 0, envWriteRemoteSafe = 0;

  // ----------------------------------
  // Collector
  // ----------------------------------
  std::function<void(IRISEXP)> collectInfo =
      [&](IRISEXP curr)
  {
    if (auto read = std::dynamic_pointer_cast<EnvReadSEXP>(curr))
    {
      if (read->hasSAFE() && read->hasFlag("MAP_INF"))
      {
        auto mapInf = read->getFlagString("MAP_INF");
        if (mapInf.find("js3$") == std::string::npos &&
            mapInf != "NA")
        {
          ss << mapInf;
        }
      }

      if (std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(read->getObj()))
      {
        envReadRemoteTotal++;
        if (read->hasSAFE())
          envReadRemoteSafe++;
      }

      return;
    }

    if (auto write = std::dynamic_pointer_cast<EnvWriteSEXP>(curr))
    {
      if (write->hasSAFE() && write->hasFlag("MAP_INF"))
      {
        auto mapInf = write->getFlagString("MAP_INF");
        if (mapInf.find("js3$") == std::string::npos &&
            mapInf != "NA")
        {
          ss << mapInf;
        }
      }

      if (std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(
              write->getLValTarget()))
      {
        envWriteRemoteTotal++;
        if (write->hasSAFE())
          envWriteRemoteSafe++;
      }
    }

    for (auto &e : curr->args)
      collectInfo(e);
  };

  // ----------------------------------
  // Fixed-point WriteBarrierReduction
  // ----------------------------------
  bool changed = true;
  int iter = 0;
  const int MAX_ITERS = 20;

  WriteBarrierReductionPass wbr;

  while (changed && iter < MAX_ITERS)
  {
    changed = false;

    for (auto &bb : fileView.bbContainerViews)
    {
      bool tainted = false;
      for (auto &ts : taintedScopes)
      {
        if (isScopeReachable(ts,
                             bb.getScopeIdx(),
                             iridiumBuildContext))
        {
          bb.tainted = true;
          tainted = true;
          break;
        }
      }
      if (tainted)
        continue;

      bool passChanged = wbr.run(bb, fileView, AM);

      if (passChanged)
      {
        changed = true;
        AM.invalidate(bb);
      }
    }

    iter++;
  }

  // ----------------------------------
  // Collect statistics
  // ----------------------------------
  for (auto &bbContView : fileView.bbContainerViews)
  {
    bbContView.cfgManager.traverseCFG(
        [&](Vertex, std::shared_ptr<BBSEXP> bb)
        {
          for (auto &stmt : bb->args)
            collectInfo(stmt);
        });
  }

  std::cout << "SAFE: "
            << (envReadRemoteSafe + envWriteRemoteSafe)
            << " TOTAL: "
            << (envReadRemoteTotal + envWriteRemoteTotal)
            << std::endl;
}

void PassManager::optimize(int level, std::set<double> taintedScopes)
{
  if (getenv("PRINT_OPT_STAT"))
  {
    printOptimizationStatus();
  }

  buildPipeline(level);

  bool changed = true;
  int iteration = 0;
  const int MAX_ITERS = 20; // TODO: Make this more visible

  while (changed && iteration < MAX_ITERS)
  {
    ScopeTimer timer("Iter " + std::to_string(iteration));
    changed = false;

    for (auto &bb : fileView.bbContainerViews)
    {
      if (isTainted(bb, taintedScopes))
      {
        continue;
      }

      for (auto &pass : pipeline)
      {
        bool passChanged = pass->run(bb, fileView, AM);

        if (passChanged)
        {
          changed = true;

          // Any IR change invalidates analyses for this container
          AM.invalidate(bb);
        }
      }
    }

    iteration++;
  }

  if (!getenv("NO_DEADBR"))
  {
    doDeadBindingRemoval(fileView);
  }

  for (auto &bbContView : fileView.bbContainerViews)
  {
    bbContView.cfgManager.traverseCFG(
        [&](Vertex v, std::shared_ptr<BBSEXP> bb)
        {
          for (auto &stmt : bb->args)
          {
            if (auto retStmt =
                    std::dynamic_pointer_cast<ReturnSEXP>(stmt))
            {
              if (auto call =
                      std::dynamic_pointer_cast<CallSiteSEXP>(
                          retStmt->getObj()))
              {
                call->setTAILCALL();
              }
            }
          }
        });
  }
}

std::shared_ptr<FileSEXP> PassManager::checkout()
{
  return fileView.checkout();
}
