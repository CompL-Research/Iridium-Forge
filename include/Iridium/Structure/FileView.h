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
  std::vector<BBContainerView> bbContainerViews;
  std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;

  SymbolTable symbolTable;

  void init();

public:
  FileView(
      std::shared_ptr<FileSEXP> file,
      std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext) : targetContainer(file), iridiumBuildContext(iridiumBuildContext)
  {
    init();
  }

  SymbolTable &getSymbolTable() { return symbolTable; }

  void dumpSymbolTable(std::ostringstream &oss);

  std::vector<BBContainerView> &getBBContainerViews() { return bbContainerViews; }

  std::shared_ptr<BBSEXP> getBB(SEXPPath path);

  std::shared_ptr<FileSEXP> checkout();
};