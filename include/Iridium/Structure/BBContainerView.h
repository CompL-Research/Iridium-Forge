#pragma once

#include "generated/IridiumTypes.h"
#include "Iridium/Globals.h"
#include "Iridium/Structure/CFGManager.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"
#include <unordered_map>

//
// A BBContainerView provides abstraction over an existing BBContainerSEXP,
//  - Provides Binding-UseDefStmt maps
//  - Provides control flow abstraction
//  - BBContainerViewBBContainerView
//
class BBContainerView
{
public:
  CFGManager cfgManager;
private:
  std::shared_ptr<BBContainerSEXP> targetContainer;
  SymbolTable & symbolTable;

  // Initialization
  void populateSymbolTable();
  void initCFG();

public:
  BBContainerView(std::shared_ptr<BBContainerSEXP> target, SymbolTable & symbolTable);

};