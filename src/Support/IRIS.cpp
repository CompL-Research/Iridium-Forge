#include "Support/IRIS.hpp"
#include "Config.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Storage/BindingsPool.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Storage/StringPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"
#include <cstdint>
#include <memory>
#include <set>
#include <stdexcept>
#include <vector>

namespace IRI_STRUCTURAL {

IRIS::IRIS(
    IRI_STORAGE::IridiumPool &pool,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iriBC,
    IRI_GEN::IRID file)
    : pool(pool) {

  //
  // Initialize scope tree
  //
  for (auto &e : iriBC) {
    auto &currScope = e.first;
    auto &bcon = e.second;
    auto parentScope = bcon->parent;
    outEdges[currScope] = parentScope;
    nodes.insert(currScope);
    nodes.insert(parentScope);

    IRI_STORAGE::IRID firstBBInScope = bcon->BB[0];

    if (bcon->isArgInitContext)
      argInitScopes.insert(currScope);

    if (bcon->kind == CF_PROP_INIT)
      propInitScopes.insert(currScope);

    //
    // If this is a try scope, add it to the tryScopes set
    //
    BBSupport bb(firstBBInScope, pool);
    if (bb.hasTryBB())
      tryScopes.insert(currScope);
  }

  //
  // Make BBContainer -> Scopes Map, also populate fastcase lookup for global
  // check
  //
  FileSupport fileSupport(file, pool);

  for (auto [bbcID, _] : fileSupport.containers()) {
    BBContainerSupport bbCont(bbcID, pool);
    BindingsSupport bindingsSupport(bbCont.getArg_Bindings(), pool);
    auto containerScope = bbCont.getScopeIDX();
    scopeHead[containerScope] = bbcID;

    for (auto [bID, _] : bindingsSupport.localBindings()) {
      IRI_GEN::EnvBindingSEXP b(bID, pool);
      allNames.insert(b.getNAME());
      scopeBindings[b.getScope()][b.getNAME()] = bID;
    }

    for (auto [rbID, _] : bindingsSupport.remoteBindings()) {
      IRI_GEN::RemoteEnvBindingSEXP rb(rbID, pool);
      auto bID = IRI_HELPERS::resolveRemoteBinding(pool, rbID);
      IRI_GEN::EnvBindingSEXP b(bID, pool);
      allNames.insert(b.getNAME()); // TODO: This is probably redundant...
      scopeBindings[containerScope][b.getNAME()] = rbID;
    }
  }
}

bool IRIS::isGlobal(StringID sid, double startScopeIDX) {

  // If no scope in the scope tree ever saw this string, it must be a
  // global
  if (!allNames.contains(sid)) {
    return true;
  }

  double currScope = startScopeIDX;
  while (currScope != -1) {
    auto scopeIt = scopeBindings.find(currScope);
    if (scopeIt != scopeBindings.end() && scopeIt->second.contains(sid)) {
      return false;
    }

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      assert(false && "Parent scope not found, error");
    }
    currScope = edgeIt->second;
  }

  return true;
}

IRI_STORAGE::IRID IRIS::resolve(StringID sid, double startScopeIDX) {

  std::vector<double> headsCrossed;
  double currScope = startScopeIDX;
  IRI_STORAGE::IRID found;

  while (true) {

    auto scopeIt = scopeBindings.find(currScope);
    if (scopeIt != scopeBindings.end() && scopeIt->second.contains(sid)) {
      found = scopeIt->second.at(sid);
      break;
    }

    if (scopeHead.contains(currScope)) {
      headsCrossed.push_back(currScope);
    }

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      throw std::runtime_error(
          "Failed to resolve env binding: scope chain broken");
    }
    currScope = edgeIt->second;

    if (currScope == -1) {
      throw std::runtime_error(
          "Failed to resolve env binding: reached top-level scope");
    }
  }

  if (headsCrossed.size() > 0) {
    while (!headsCrossed.empty()) {
      auto currHead = headsCrossed.back();
      headsCrossed.pop_back();
      found = IRI_GEN::RemoteEnvBindingSEXP::create(pool, found, false, false,
                                                    false, -1);

      commitList[currHead].push_back(found);
      scopeBindings[currHead][sid] = found;
    }
  }

  return found;
}

void IRIS::commit() {
  for (auto &e : commitList) {
    int headScope = e.first;
    auto &currentCommits = e.second;

    BBContainerSupport bbc(scopeHead[headScope], pool);
    BindingsSupport bindings(bbc.getArg_Bindings(), pool);
    auto remoteBindingsID = bindings.getArg_RemoteBindings();
    size_t oldArgs = pool[remoteBindingsID].num_args;

    for (size_t i = 0; i < currentCommits.size(); i++) {
      IRI_GEN::RemoteEnvBindingSEXP rb(currentCommits[i], pool);
      rb.setREFIDX(oldArgs + i);
    }
    pool.add_args_to_end(remoteBindingsID, currentCommits);
  }
  commitList.clear();
  // Only added becauase I was unsure about what clear does,
  // just to be safe let this be for now...
  if (!commitList.empty())
    throw std::runtime_error("Expected commit List to be empty");
}

bool IRIS::hasScopePath(double startScope, double targetScope) {
  double currScope = startScope;
  while (currScope != -1) {
    if (currScope == targetScope) {
      return true;
    }

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      assert(false && "Parent scope not found, error");
    }
    currScope = edgeIt->second;
  }
  return false;
}

double IRIS::isArgInitScope(double argInitScope) {
  return argInitScopes.contains(argInitScope);
}

bool IRIS::isEnclosedInAPropInitScope(double currScope) {
  for (auto & pis : propInitScopes) {
    if (hasScopePath(currScope, pis)) return true;
  }
  return false;
}

bool IRIS::isTopLevelScope(double startScope) {
  auto edgeIt = outEdges.find(startScope);
  if (edgeIt == outEdges.end()) {
    assert(false && "Parent scope not found, error");
  }
  return edgeIt->second == -1;
}

bool IRIS::mayReadFromATaintedScope(double startScope) {
  for (auto & ts : taintedScopes) {
    // std::cout << "Check: " << startScope << " --> " << ts << std::endl;
    if (hasScopePath(startScope, ts)) return true;
  }
  return false;
}

void IRIS::addEvalRemoteBindingsToParentClosure(double startScope) {
  if (taintedScopes.contains(startScope))
    return;

  taintedScopes.insert(startScope);

  //
  // 1. Add all (possibly) read bindings
  //    to the current closure scope.
  // 2. Populate remote env reads in the closure frame
  //
  double closureScope = getEnclosingClosureScope(startScope);

  std::set<StringID> shadowedReads;
  std::set<StringID> pollutedReads;

  double currScope = closureScope;
  bool crossedClosureScope = false;
  while (currScope != -1) {
    if (scopeBindings.contains(currScope)) {
      for (auto &[sID, bID] : scopeBindings.at(currScope)) {
        if (pool[bID].tag == IRI_GEN::EnvBinding) {
          if (crossedClosureScope) {
            pollutedReads.insert(sID);
          } else {
            shadowedReads.insert(sID);
          }
        }
      }
    }

    if (scopeHead.contains(currScope))
      crossedClosureScope = true;

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      assert(false && "Parent scope not found, error");
    }
    currScope = edgeIt->second;
  }

  for (auto &sid : pollutedReads) {
    if (shadowedReads.contains(sid)) continue;
    resolve(sid, startScope);
  }

  commit();
}

double IRIS::getJSEvalLookupREFIDX(double scope, double parentScope) {

  if (!scopeHead.contains(parentScope)) {
    throw std::runtime_error(
        "getJSEvalLookupREFIDX called on a non container scope");
  }

  if (!hasScopePath(scope, parentScope)) {
    throw std::runtime_error("getJSEvalLookupREFIDX scope is invalid");
  }

  double currScope = scope;

  do {
    auto scopeIt = scopeBindings.find(currScope);
    if (scopeIt != scopeBindings.end() && scopeIt->second.size() > 0) {
      double largestREFIDX = -1;
      // Found a scope with >1 bindings
      for (auto &b : scopeIt->second) {
        IRI_STORAGE::IRID bID = b.second;
        if (pool[bID].tag == IRI_GEN::EnvBinding) {
          IRI_GEN::EnvBindingSEXP ebSEXP(bID, pool);
          if (ebSEXP.getREFIDX() > largestREFIDX)
            largestREFIDX = ebSEXP.getREFIDX();
        }
      }
      if (largestREFIDX == -1) {
        assert(scopeHead.contains(currScope));
      }
      return largestREFIDX + 1;
      break;
    }

    if (currScope == parentScope)
      return 0;

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      throw std::runtime_error(
          "Failed to resolve env binding: scope chain broken");
    }
    currScope = edgeIt->second;
  } while (true);
}

double IRIS::getEnclosingClosureScope(double startScope) {
  assert(startScope != -1);

  double currScope = startScope;
  while (true) {
    if (scopeHead.contains(currScope))
      return currScope;

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      assert(false && "Parent scope not found, error");
    }
    currScope = edgeIt->second;
  }
  throw std::runtime_error("Failed to resolve parent closure scope");
  return -1;
}

double IRIS::getEnclosingThrowScope(double startScope) {
  assert(startScope != -1);

  double currScope = startScope;
  while (true) {
    if (scopeHead.contains(currScope) || tryScopes.contains(currScope))
      return currScope;

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      assert(false && "Parent scope not found, error");
    }
    currScope = edgeIt->second;
  }

  return -1;
}

void IRIS::initializeBindingsPool() {
  if (!commitList.empty())
    throw std::runtime_error(
        "Expected commitList to be empty before creating the bindingsPool");

  // Get all the bindings
  std::vector<IRI_STORAGE::IRID> bindings;
  for (auto &e : scopeBindings) {

    for (auto &b : e.second) {
      IRI_STORAGE::IRID currID = b.second;
      IRI_STORAGE::IRID currTAG = pool[currID].tag;
      // EnvBindings for top level bindings in modules, need to be added
      // explicitly as they are not part of the local bindings list...
      if (currTAG == IRI_GEN::RemoteEnvBinding) {
        IRI_GEN::RemoteEnvBindingSEXP rb(currID, pool);
        if (rb.hasMODULETOPLEVELBINDING()) {
          IRI_STORAGE::IRID targetID = rb.getArg_ParentReference();
          IRI_GEN::EnvBindingSEXP bb(targetID, pool);
          bindings.push_back(targetID);
        }
      }
      bindings.push_back(b.second);
    }
  }

  // Initialize the bindings pool
  bindingsPool = std::make_unique<IRI_STORAGE::BindingsPool>(bindings.size());

  //
  // Allocate the stub before inserting any bindings
  // - Stub always occupies the 0th index of the pool.
  // - If a back offset for any node is zero, that means that its missing.
  //
  bindingsPool->allocateStub();

  // Store all the bindings in the pool
  for (auto &bID : bindings) {
    bindingsPool->allocate(pool, bID);
  }

  // Make child parent links
  for (auto &bID : bindings) {
    auto currTag = pool[bID].tag;
    if (currTag == IRI_GEN::RemoteEnvBinding) {
      uint32_t childRemoteIdx =
          IRI_STORAGE::BindingsPool::getBPoolOffsetForEnvBinding(pool, bID);

      // Verify we are working on the right object...
      auto &bindingMeta = (*bindingsPool)[childRemoteIdx];
      assert(bindingMeta.tag == currTag && bindingMeta.ID == bID &&
             "Binding pool may be corrupted");

      uint32_t parentIdx =
          IRI_STORAGE::BindingsPool::getBPoolOffsetForEnvBinding(
              pool, IRI_GEN::RemoteEnvBindingSEXP(bID, pool)
                        .getArg_ParentReference());

      (*bindingsPool).addCapture(parentIdx, childRemoteIdx);
    }
  }
}

std::vector<IRI_STORAGE::IRID> IRIS::getBindingsToMoveToHeap(double startScope,
                                                             double endScope) {
  if (!hasScopePath(startScope, endScope))
    return {};

  std::vector<IRI_STORAGE::IRID> res;

  double currScope = startScope;
  do {
    auto scopeIt = scopeBindings.find(currScope);
    if (scopeIt != scopeBindings.end()) {
      auto &bindingsMapAtScope = scopeIt->second;
      for (auto &b : bindingsMapAtScope) {
        auto bID = b.second;
        if (pool[bID].tag == IRI_GEN::EnvBinding &&
            getBindingsMetaView(bID).forwardOffsets.size() > 0) {
          res.push_back(bID);
        }
      }
    }

    if (startScope == endScope)
      break;

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      assert(false && "Parent scope not found, error");
    }
    currScope = edgeIt->second;
  } while (currScope != endScope);

  return res;
}

const IRI_STORAGE::BindingMeta &
IRIS::getBindingsMetaView(IRI_STORAGE::IRID id) const {
  auto offset =
      IRI_STORAGE::BindingsPool::getBPoolOffsetForEnvBinding(pool, id);
  return (*bindingsPool)[offset];
}

void IRIS::dumpCommitList(std::ostream &oss) const {
  oss << "=== IRIS Pending Commit List ===\n";
  if (commitList.empty()) {
    oss << "  [Empty]\n";
    return;
  }
  for (const auto &[headScope, commits] : commitList) {
    oss << "  BBContainer Head Scope " << headScope << " (" << commits.size()
        << " pending remote bindings):\n";
    for (size_t i = 0; i < commits.size(); ++i) {
      oss << "    [" << i << "] IRID: " << commits[i] << "\n";
    }
  }
}

void IRIS::dumpBindingsAtScope(std::ostream &oss, double currScope) const {
  if (!scopeBindings.contains(currScope)) {
    oss << "[No bindings]";
    return;
  }

  for (auto &[sID, bID] : scopeBindings.at(currScope)) {
    bool captured = false;
    if (bindingsPool != nullptr)
      captured = getBindingsMetaView(bID).forwardOffsets.size() > 0;
    oss << pool.strings.get(sID) << "@" << bID << ":"
        << (pool[bID].tag == IRI_GEN::RemoteEnvBinding ? "R" : "L")
        << (bindingsPool != nullptr && captured ? "^" : "") << " ";
  }
}

void IRIS::dumpAllNames(std::ostream &oss) const {
  oss << "=== IRIS : All declared names ===\n";
  if (allNames.empty()) {
    oss << "  [Empty]\n";
    return;
  }
  oss << "  ";
  for (const auto &nameID : allNames) {
    oss << pool.strings.get(nameID) << " ";
  }
  oss << "\n";
}

void IRIS::dumpFlat(std::ostream &oss, int indentLevel) const {
  oss << "\n=============================================\n";
  oss << "            IRIS STATE DUMP                  \n";
  oss << "=============================================\n";
  dumpScopeTree(oss, indentLevel);
  dumpAllNames(oss);
  dumpCommitList(oss);
  if (bindingsPool)
    bindingsPool->dump(oss);
  else
    oss << "[BindingsPool not yet initialized]\n";
  oss << "=============================================\n\n";
}

void IRIS::dumpScopeTree(std::ostream &oss, int indentLevel) const {
  oss << "=== IRIS Scope Tree ===\n";
  if (nodes.empty()) {
    oss << "  [Empty]\n";
    return;
  }

  // 1. Build an adjacency list (Parent -> Children) for O(N) lookup
  std::unordered_map<double, std::vector<double>> childrenMap;
  std::vector<double> roots;

  for (double node : nodes) {
    auto it = outEdges.find(node);
    if (it != outEdges.end() && it->second != -1.0) {
      childrenMap[it->second].push_back(node);
    } else if (it != outEdges.end() && it->second == -1.0) {
      roots.push_back(node);
    }
  }

  // Sort for deterministic output
  std::sort(roots.begin(), roots.end());
  for (auto &[parent, children] : childrenMap) {
    std::sort(children.begin(), children.end());
  }

  // 2. Recursively print with fancy connectors
  for (size_t i = 0; i < roots.size(); ++i) {
    bool isLast = (i == roots.size() - 1);
    printNode(oss, roots[i], "", isLast, childrenMap);
  }
}

void IRIS::printNode(
    std::ostream &oss, double node, std::string prefix, bool isLast,
    const std::unordered_map<double, std::vector<double>> &childrenMap) const {

  // Print current node with branching characters
  oss << prefix << (isLast ? "└── " : "├── ")
      << (taintedScopes.contains(node) ? "(†)" : "") << node
      << (scopeHead.contains(node) ? "(*)" : "") << " : ";
  dumpBindingsAtScope(oss, node);
  oss << "\n";

  // Update prefix for children
  std::string newPrefix = prefix + (isLast ? "    " : "│   ");

  if (childrenMap.count(node)) {
    const auto &children = childrenMap.at(node);
    for (size_t i = 0; i < children.size(); ++i) {
      bool childIsLast = (i == children.size() - 1);
      printNode(oss, children[i], newPrefix, childIsLast, childrenMap);
    }
  }
}

} // namespace IRI_STRUCTURAL
