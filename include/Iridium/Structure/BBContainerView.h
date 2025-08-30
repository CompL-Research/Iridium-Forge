#pragma once

#include "generated/IridiumTypes.h"


// 
// A BBContainerView provides abstraction over an existing BBContainerSEXP,
//  - Provides Binding-UseDefStmt maps
//  - Provides control flow abstraction
//  - BBContainerViewBBContainerView
// 
class BBContainerView
{
private:
  std::shared_ptr<BBContainerSEXP> targetContainer;
  std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::vector<IRISEXP>> bDUStmtMap;
  std::unordered_map<std::shared_ptr<RemoteEnvBindingSEXP>, std::vector<IRISEXP>> bDUStmtMapTopLevelModuleBindings;

  // For all local bindings, find the statements where it is used
  void initBDUChains();

public:
  BBContainerView(std::shared_ptr<BBContainerSEXP> target);
  
  void addBDUStmtElement(std::shared_ptr<EnvBindingSEXP> binding, IRISEXP stmt);
  void addBDUStmtElement(std::shared_ptr<RemoteEnvBindingSEXP> binding, IRISEXP stmt);

  void dumpBDUChains(std::ostringstream &oss, bool compressed = false, int indent = 0);

};