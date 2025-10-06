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
  BindingsView bindingsView;
  CFGManager cfgManager;
  bool tainted = false;
  
private:
  SymbolTable & symbolTable;
  std::unordered_map<int, IRIBUILDCONTEXT> & iridiumBuildContext;

public:
  BBContainerView(std::shared_ptr<BBContainerSEXP> target, SymbolTable & symbolTable, std::unordered_map<int, IRIBUILDCONTEXT> & iridiumBuildContext);

  BBContainerView(
    std::shared_ptr<BBContainerSEXP> targetContainer,
    CFGManager & cfgManager,
    BindingsView & bindingsView,
    SymbolTable & symbolTable,
    std::unordered_map<int, IRIBUILDCONTEXT> & iridiumBuildContext,
    std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::shared_ptr<EnvBindingSEXP>> & localIndirectionMap,
    std::unordered_map<std::shared_ptr<RemoteEnvBindingSEXP>, std::shared_ptr<RemoteEnvBindingSEXP>> & remoteIndirectionMap,
    double & sinIDX
  ) : 
  targetContainer(targetContainer),
  bindingsView(bindingsView.clone(localIndirectionMap, remoteIndirectionMap)),
  cfgManager(cfgManager.clone(localIndirectionMap, remoteIndirectionMap, sinIDX)),
  symbolTable(symbolTable),
  iridiumBuildContext(iridiumBuildContext) {}

  void initCFG();
  void refreshSymbolTable();
  void populateSymbolTable();
  
  double getScopeIdx() { return targetContainer->getScopeIDX(); }
  double getStartBBIDX() { return targetContainer->getStartBBIDX(); }

  std::set<IRISEXP> getAllStackBindings();
  
  std::set<IRISEXP> getUncapturedStackBindings();

  std::set<std::shared_ptr<EnvBindingSEXP>> getCapturedStackBindings();

  bool hasImplicitBindings();

  std::shared_ptr<BBContainerSEXP> checkout();

  BBContainerView clone(double & sinIDX)
  {
    std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::shared_ptr<EnvBindingSEXP>> localIndirectionMap;
    std::unordered_map<std::shared_ptr<RemoteEnvBindingSEXP>, std::shared_ptr<RemoteEnvBindingSEXP>> remoteIndirectionMap;
    BBContainerView res(targetContainer, cfgManager, bindingsView, symbolTable, iridiumBuildContext, localIndirectionMap, remoteIndirectionMap, sinIDX);
    return res;
  }

  json getDebugJSON(const std::string & title);
};