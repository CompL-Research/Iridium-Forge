#pragma once

// #include "Iridium/Globals.h"
// #include "Iridium/Structure/FileView.h"
// #include "generated/IridiumTypes.h"
// #include <sstream>
#include "Iridium/Analysis/Domains/AnalysisManager.h"
#include "Iridium/OptimizationPasses/BBPass.h"

std::vector<std::unique_ptr<BBPass>> pipeline;
AnalysisManager AM;

class PassManager {
  std::vector<std::function<void(FileView &, std::unordered_map<int, IRIBUILDCONTEXT>)>> passes;
  FileView &fileView;
  std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;

  void buildPipeline(int level);
  bool isTainted(BBContainerView &bb, const std::set<double> &taintedScopes);

public:
  PassManager(FileView & fs, std::unordered_map<int, IRIBUILDCONTEXT> bc) : fileView(fs), iridiumBuildContext(bc) {}

  void justAnalysis(std::stringstream &, std::set<double>);
  void optimize(int level, std::set<double> taintedScopes);

  std::shared_ptr<FileSEXP> checkout();
};
