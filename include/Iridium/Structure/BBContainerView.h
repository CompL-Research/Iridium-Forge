#pragma once

#include "generated/IridiumTypes.h"
#include "Iridium/Globals.h"
#include "Iridium/Structure/CFGManager.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"
#include <unordered_map>

//
// A BBContainerView provides abstraction over an existing BBContainerSEXP,
//  - Provides control flow abstraction
//  - BBContainerViewBBContainerView
//
class BBContainerView
{
public:
  std::shared_ptr<BBContainerSEXP> targetContainer;
  CFGManager cfgManager;
  double scopeIdx;
private:
  SymbolTable & symbolTable;

public:
  BBContainerView(std::shared_ptr<BBContainerSEXP> target, SymbolTable & symbolTable);

  void initCFG();
  void populateSymbolTable();

};