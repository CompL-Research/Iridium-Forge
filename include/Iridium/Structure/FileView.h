#pragma once

#include "generated/IridiumTypes.h"
#include "Iridium/Globals.h"
#include "Iridium/Structure/BBContainerView.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"
#include <unordered_map>

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

  std::vector<BBContainerView> &getBBContainerViews() { return bbContainerViews; }

  std::shared_ptr<BBSEXP> getBB(SEXPPath path);

  std::shared_ptr<FileSEXP> checkout();
};