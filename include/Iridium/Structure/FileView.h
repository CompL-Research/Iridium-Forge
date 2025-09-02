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

  SymbolTable symbolTable;

  void init();

public:

  FileView(std::shared_ptr<FileSEXP> file) : targetContainer(file) { init(); }

  SymbolTable & getSymbolTable() { return symbolTable; }

  void dumpSymbolTable(std::ostringstream &oss);

  std::vector<BBContainerView> & getBBContainerViews() { return bbContainerViews; }

};