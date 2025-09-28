#pragma once

#include "generated/IridiumTypes.h"
#include "Iridium/Globals.h"
#include "Iridium/Structure/CFGManager.h"
#include "Iridium/Structure/BindingsView.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"
#include <unordered_map>
#include "external/json.hpp"
using json = nlohmann::json;

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
  BindingsView bindingsView;
  
private:
  SymbolTable & symbolTable;
  std::unordered_map<int, IRIBUILDCONTEXT> & iridiumBuildContext;

public:
  BBContainerView(std::shared_ptr<BBContainerSEXP> target, SymbolTable & symbolTable, std::unordered_map<int, IRIBUILDCONTEXT> & iridiumBuildContext);

  void initCFG();
  void populateSymbolTable();
  
  double getScopeIdx() { return targetContainer->getScopeIDX(); }
  double getStartBBIDX() { return targetContainer->getStartBBIDX(); }

  std::set<IRISEXP> getAllStackBindings();
  
  std::set<IRISEXP> getUncapturedStackBindings();

  std::set<std::shared_ptr<EnvBindingSEXP>> getCapturedStackBindings();

  std::shared_ptr<BBContainerSEXP> checkout();

  json getDebugJSON(const std::string & title);
};