#pragma once
#include "Iridium/Globals.h"
#include "Iridium/Structure/FileView.h"
#include "generated/IridiumTypes.h"
#include <sstream>

class PassManager {
public:
  PassManager(FileView & fs, std::unordered_map<int, IRIBUILDCONTEXT> bc) : fileView(fs), iridiumBuildContext(bc) {}

  void justAnalysis(std::stringstream &, std::set<double>);
  void optimize(int level, std::set<double> taintedScopes);

  void pta(int level, std::set<double> taintedScopes);

  std::shared_ptr<FileSEXP> checkout();

private:
  std::vector<std::function<void(FileView &, std::unordered_map<int, IRIBUILDCONTEXT>)>> passes;
  FileView & fileView;
  std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;
};