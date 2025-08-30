#pragma once

#include "generated/IridiumTypes.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"

// CFG
using CFG = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::directedS,
    std::shared_ptr<BBSEXP>
>;

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

  std::shared_ptr<BBSEXP> startBB;
  std::unordered_map<double, std::shared_ptr<BBSEXP>> bbIdMap;
  std::unordered_map<std::shared_ptr<BBSEXP>, size_t> vertexMap;
  CFG controlFlowGraph;

  // For all local bindings, find the statements where it is used
  void initBDUChains();

  void initCFG();

public:
  BBContainerView(std::shared_ptr<BBContainerSEXP> target);
  
  void addBDUStmtElement(std::shared_ptr<EnvBindingSEXP> binding, IRISEXP stmt);
  void addBDUStmtElement(std::shared_ptr<RemoteEnvBindingSEXP> binding, IRISEXP stmt);

  void dumpBDUChains(std::ostringstream &oss, bool compressed = false, int indent = 0);
  void dumpCFGDOT(std::string filePath);
};