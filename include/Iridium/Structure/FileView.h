#pragma once

#include "generated/IridiumTypes.h"
#include "Iridium/Globals.h"
#include "Iridium/Structure/BBContainerView.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"
#include <unordered_map>
#include "Iridium/Analysis/Domains/TDZA.h"

//
// A FileView provides abstraction over an existing FileSEXP,
//  - It contains a list of BBContainerView, an abstraction over the BBContainerSEXP
//  -
//
class FileView
{
private:
  std::shared_ptr<FileSEXP> targetContainer;
  std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;

  void init();

public:
  std::vector<BBContainerView> bbContainerViews;
  std::set<IRISEXP> safelyCapturedBindings;

  SymbolTable symbolTable;

  FileView(
      std::shared_ptr<FileSEXP> file,
      std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext) : targetContainer(file), iridiumBuildContext(iridiumBuildContext)
  {
    init();
  }

  void dumpSymbolTable(std::ostringstream &oss);

  void deleteBinding(std::shared_ptr<EnvBindingSEXP> binding);

  void refreshSymbolTable();

  void updateSafelyCapturedBindingsSet(double bbIDX, TDZA val);

  std::vector<BBContainerView> &getBBContainerViews() { return bbContainerViews; }

  BBContainerView &getContainer(double idx) {
    for (auto & bbCont : bbContainerViews)
    {
      if (bbCont.targetContainer->getStartBBIDX() == idx) return bbCont;
    }
    throw std::runtime_error("Failed to fetch the desired container");
  }

  BBContainerView &getTopLevelContainer() {
    for (auto & bbCont : bbContainerViews)
    {
      if (bbCont.targetContainer->hasTopLevel()) return bbCont;
    }
    throw std::runtime_error("Failed to fetch top level container");
  }

  std::shared_ptr<BBSEXP> getBB(SEXPPath path);

  std::shared_ptr<FileSEXP> checkout();
};