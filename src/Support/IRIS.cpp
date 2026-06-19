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
#include "Support/ClosureTree.hpp"
#include "Support/FileSupport.hpp"
#include <cassert>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>
#include <vector>

namespace IRI_STRUCTURAL {
using namespace IRI_STORAGE;
IRIS::IRIS(
    IRI_STORAGE::IridiumPool &pool,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iriBC,
    IRI_GEN::IRID file)
    : pool(pool) {

  //
  // Make BBContainer -> Scopes Map, also populate fastcase lookup for global
  // check
  //
  FileSupport fileSupport(file, pool);

  for (auto [bbcID, _] : fileSupport.containers()) {
    BBContainerSupport bbCont(bbcID, pool);
    auto containerScope = bbCont.getScopeIDX();
    scopeHead[containerScope] = bbcID;
  }

  //
  // Initialize scope tree
  //
  for (auto &e : iriBC) {
    auto &currScope = e.first;
    auto &bcon = e.second;
    auto parentScope = bcon->parent;

    if (bcon->tryContext.has_value()) {
      auto lCon = bcon->tryContext.value();
      if (lCon.tryScopeIDX > -1) {
        double redirectBBIDX = -1;
        if (lCon.udCatchIDX > -1) {
          redirectBBIDX = lCon.udCatchIDX;
        } else {
          redirectBBIDX = lCon.imCatchIDX;
        }
        assert(redirectBBIDX != -1);
        exceptionEdgeRedirect[lCon.tryScopeIDX] = redirectBBIDX;
      }

      if (lCon.udCatchScopeIDX > -1) {
        assert(lCon.imCatchIDX != -1);
        exceptionEdgeRedirect[lCon.udCatchScopeIDX] = lCon.imCatchIDX;
      }

      if (lCon.finalizerIDX > -1) {
        assert(lCon.finalizerRetIDX != -1);
        finalizerRetMap[lCon.finalizerIDX] = lCon.finalizerRetIDX;
      }
    }

    // Populate Tree
    outEdges[currScope] = parentScope;
    inEdges[parentScope].insert(currScope);
    nodes.insert(currScope);
    nodes.insert(parentScope);

    BBSupport bb(bcon->BB[0], pool);

    if (bcon->isArgInitContext)
      argInitScopes.insert(currScope);

    if (bcon->kind == CF_PROP_INIT)
      propInitScopes.insert(currScope);

    if (bb.hasTryBB())
      tryScopes.insert(currScope);

    // Populate args
    size_t numArgs = bcon->args.size();
    if (numArgs > 0) {
      assert(scopeHead.contains(currScope));
    }
    for (int k = 0; k < bcon->args.size(); k++) {
      IRI_FLAG kind = JSARG;
      if (k + 1 == bcon->args.size() && bcon->hasRestArgs) {
        kind = JSRESTARG;
      }

      IRI_STORAGE::StringID bName = pool.strings.intern(bcon->args[k]);

      auto lb =
          EnvBindingSEXP::create(pool, bName, kind == JSARG, kind == JSRESTARG,
                                 false, false, false, k, currScope, -1, -1);

      allNames.insert(bName);
      scopeBindings[currScope][bName] = lb;
      bindingsPool->allocate(pool, lb);

      argsAtScope[currScope].push_back(lb);
    }
  }

  // Set value of top level scooe
  for (auto &e : outEdges) {
    if (e.second == -1)
      topLevelScope = e.first;
  }

  if (topLevelScope == -1)
    throw std::runtime_error("IRIS, top level scope not found");
}

IRI_STORAGE::BindingMeta &IRIS::declareScriptBinding(double scope,
                                                     StringID name,
                                                     IRI_GEN::IRI_FLAG flag) {

  if (scope != topLevelScope) {
    throw std::runtime_error(
        "script binding declaration at non top level scope: " +
        std::string(pool.strings.get(name)));
  }
  if (scriptBindings.contains(name)) {
    IRID existingBinding = scriptBindings[name];
    ScriptBindingSEXP sb(existingBinding, pool);
    if (!sb.hasJSVAR()) {
      throw std::runtime_error("Duplicate script binding declaration: " +
                               std::string(pool.strings.get(name)));
    }
    return (*this)[existingBinding];
  } else {
    bool JSLET = flag == IRI_FLAG::JSLET;
    bool JSCONST = flag == IRI_FLAG::JSCONST;
    bool JSVAR = flag == IRI_FLAG::JSVAR;

    assert(JSLET || JSCONST || JSVAR);
    IRID newBinding =
        ScriptBindingSEXP::create(pool, name, JSLET, JSCONST, JSVAR, -1);
    scriptBindings[name] = newBinding;
    bindingsPool->allocate(pool, newBinding);
    return (*this)[newBinding];
  }
}

IRI_STORAGE::BindingMeta &IRIS::declareLBinding(double scope, StringID name,
                                                IRI_GEN::IRI_FLAG flag) {
  auto &bindingsAtScope = scopeBindings[scope];
  if (bindingsAtScope.contains(name)) {
    IRID existingBinding = bindingsAtScope[name];
    EnvBindingSEXP eb(existingBinding, pool);
    auto &meta = (*this)[existingBinding];

    if (!meta.isIMPLICITOVERRIDEABLE) {
      if (eb.hasJSVAR() || eb.hasJSARG() || eb.hasJSRESTARG()) {
        return meta;
      } else {
        throw std::runtime_error("Duplicate binding declaration at scope: " +
                                 std::string(pool.strings.get(name)));
      }
    }
    meta.tombstone = true;
  }
  allNames.insert(name);
  StringID NAME = name;
  bool JSARG = flag == IRI_FLAG::JSARG;
  bool JSRESTARG = flag == IRI_FLAG::JSRESTARG;
  bool JSLET = flag == IRI_FLAG::JSLET;
  bool JSCONST = flag == IRI_FLAG::JSCONST;
  bool JSVAR = flag == IRI_FLAG::JSVAR;
  assert(JSARG || JSRESTARG || JSLET || JSCONST || JSVAR);
  double REFIDX = -1;
  double SCOPE = scope;
  double NEXT = -1;
  double LINK = -1;
  IRID newBinding = IRI_GEN::EnvBindingSEXP::create(
      pool, NAME, JSARG, JSRESTARG, JSLET, JSCONST, JSVAR, REFIDX, SCOPE, NEXT,
      LINK);
  bindingsAtScope[name] = newBinding;
  bindingsPool->allocate(pool, newBinding);
  return (*this)[newBinding];
}

IRI_STORAGE::BindingMeta &IRIS::declareRBinding(double scope, StringID name,
                                                IRI_GEN::IRI_FLAG flag,
                                                IRI_GEN::IRI_FLAG moduleFlag) {
  auto &bindingsAtScope = scopeBindings[scope];
  if (scope != topLevelScope) {
    throw std::runtime_error(
        "Remote binding declaration at non top level scope: " +
        std::string(pool.strings.get(name)));
  }
  if (bindingsAtScope.contains(name)) {
    IRID existingBinding = bindingsAtScope[name];
    RemoteEnvBindingSEXP rb(existingBinding, pool);
    EnvBindingSEXP eb(rb.getArg_ParentReference(), pool);

    if (eb.hasJSVAR()) {
      return (*this)[existingBinding];
    }
    throw std::runtime_error("Duplicate remote binding declaration at scope: " +
                             std::string(pool.strings.get(name)));
  } else {
    allNames.insert(name);
    StringID NAME = name;
    bool JSARG = flag == IRI_FLAG::JSARG;
    bool JSRESTARG = flag == IRI_FLAG::JSRESTARG;
    bool JSLET = flag == IRI_FLAG::JSLET;
    bool JSCONST = flag == IRI_FLAG::JSCONST;
    bool JSVAR = flag == IRI_FLAG::JSVAR;
    double REFIDX = -1;
    double SCOPE = scope;
    double NEXT = -1;
    double LINK = -1;
    IRID store = IRI_GEN::EnvBindingSEXP::create(pool, NAME, JSARG, JSRESTARG,
                                                 JSLET, JSCONST, JSVAR, REFIDX,
                                                 SCOPE, NEXT, LINK);
    bindingsPool->allocate(pool, store);

    assert(moduleFlag == IRI_FLAG::MODULE || moduleFlag == IRI_FLAG::MODULEI ||
           moduleFlag == IRI_GEN::MODULENSI);
    IRID moduleBinding = RemoteEnvBindingSEXP::create(
        pool, store, moduleFlag == MODULE, moduleFlag == MODULEI,
        moduleFlag == MODULENSI, -1, -1);

    bindingsAtScope[name] = moduleBinding;
    bindingsPool->allocate(pool, moduleBinding);
    return (*this)[moduleBinding];
  }
}

void IRIS::addClosureAtScope(double scope, IRI_STORAGE::IRID closure) {
  closuresAtScope[scope].push_back(closure);
}

bool IRIS::hasBinding(StringID name, double currScope) {
  auto scopeIt = scopeBindings.find(currScope);
  if (scopeIt != scopeBindings.end() && scopeIt->second.contains(name)) {
    return true;
  }
  return false;
}

void IRIS::removeBinding(StringID name, double currScope) {
  assert(hasBinding(name, currScope));
  scopeBindings[currScope].erase(name);
}

double IRIS::getLexicalScope(double currScope) {
  auto scopeIt = outEdges.find(currScope);
  if (scopeIt == outEdges.end()) {
    throw std::runtime_error("Failed to fetch lexical scope");
  }
  return scopeIt->second;
}

IRI_STORAGE::BindingMeta &IRIS::resolve(StringID sid, double startScopeIDX) {
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
    // If edgeIt->second == -1, we have reached a Global, declare and return
    if (edgeIt->second == -1) {
      if (scriptBindings.contains(sid)) {
        return (*this)[scriptBindings[sid]];
      } else if (globalBindings.contains(sid)) {
        return (*this)[globalBindings[sid]];
      } else {
        IRID newBinding = GlobalBindingSEXP::create(pool, sid, -1);
        globalBindings[sid] = newBinding;
        bindingsPool->allocate(pool, newBinding);
        return (*this)[newBinding];
      }
      break;
    } else {
      currScope = edgeIt->second;
    }
  }

  if (headsCrossed.size() > 0) {
    while (!headsCrossed.empty()) {
      auto currHead = headsCrossed.back();
      headsCrossed.pop_back();
      IRID parentID = found;
      found = RemoteEnvBindingSEXP::create(pool, found, false, false, false, -1,
                                           -1);
      scopeBindings[currHead][sid] = found;
      bindingsPool->allocate(pool, found);
      BindingMeta::connect((*this)[found], (*this)[parentID]);
    }
  }

  return (*this)[found];
}

void IRIS::registerDirectEval(IRI_STORAGE::IRID dEval, double startScope,
                              double endScope) {
  directEvals.push_back(std::make_tuple(dEval, startScope, endScope));
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

// Wrapper method to fetch filtered IRIDs
static std::vector<IRI_STORAGE::IRID> getEnvBindings(
    const std::unordered_map<
        double, std::unordered_map<StringID, IRI_STORAGE::IRID>> &scopeBindings,
    auto scope, auto &pool) {
  std::vector<IRI_STORAGE::IRID> matchedBindings;

  auto scopeIt = scopeBindings.find(scope);
  if (scopeIt == scopeBindings.end()) {
    return matchedBindings; // Return empty if scope doesn't exist
  }

  const auto &innerMap = scopeIt->second;
  for (const auto &[stringId, irid] : innerMap) {
    if (pool[irid].tag == EnvBinding) {
      matchedBindings.push_back(irid);
    }
  }

  return matchedBindings;
}

// Wrapper method to fetch filtered IRIDs
static std::vector<IRI_STORAGE::IRID> getRemoteEnvBindings(
    const std::unordered_map<
        double, std::unordered_map<StringID, IRI_STORAGE::IRID>> &scopeBindings,
    auto scope, auto &pool) {
  std::vector<IRI_STORAGE::IRID> matchedBindings;

  auto scopeIt = scopeBindings.find(scope);
  if (scopeIt == scopeBindings.end()) {
    return matchedBindings; // Return empty if scope doesn't exist
  }

  const auto &innerMap = scopeIt->second;
  for (const auto &[stringId, irid] : innerMap) {
    if (pool[irid].tag == RemoteEnvBinding) {
      matchedBindings.push_back(irid);
    }
  }

  return matchedBindings;
}

void IRIS::populateCClosuresInTree() {
  if (!pool.closureTree)
    throw std::runtime_error(
        "Expected closure Tree to exist before populating CClosures");

  for (auto &[currHead, bbcID] : scopeHead) {
    std::vector<IRID> cBindings = closuresAtScope.contains(currHead)
                                      ? closuresAtScope[currHead]
                                      : std::vector<IRID>();
    for (auto &b : cBindings) {
      PoolBindingSEXP pb(b, pool);
      pool.closureTree->addEdgeFromScopeToBBIDX(currHead, pb.getStartBBIDX());
    }
  }
}

void IRIS::commit() {
  // For each
  for (auto &[currHead, bbcID] : scopeHead) {
    double lIDX = 0;
    double rIDX = 0;
    double riIDX = 0;
    double rlIDX = 0;

    std::unordered_map<double, std::vector<double>> lbAtScope;
    std::vector<IRID> lBindings = argsAtScope[currHead];
    std::vector<IRID> rBindings;
    std::vector<IRID> cBindings = closuresAtScope.contains(currHead)
                                      ? closuresAtScope[currHead]
                                      : std::vector<IRID>();
    std::function<void(double, bool)> handleFrameBindings =
        [&](double currScope, bool allowHead) {
          if (scopeHead.contains(currScope) && !allowHead)
            return;
          std::vector<IRID> LB = getEnvBindings(scopeBindings, currScope, pool);
          std::vector<IRID> RB =
              getRemoteEnvBindings(scopeBindings, currScope, pool);

          for (size_t i = 0; i < LB.size(); i++) {
            IRID lbID = LB[i];
            EnvBindingSEXP lb(lbID, pool);
            if (lb.hasJSARG() || lb.hasJSRESTARG())
              continue;
            double NEXT;
            double REFIDX;
            if (i == 0) {
              if (currScope == currHead) {
                NEXT = -1;
              } else {
                assert(outEdges.contains(currScope));
                double parentScope = outEdges[currScope];
                while (true) {
                  if (lbAtScope[parentScope].size() > 0) {
                    NEXT = lbAtScope[parentScope].back();
                    break;
                  }

                  if (parentScope == currHead) {
                    NEXT = -1;
                    break;
                  } else {
                    assert(outEdges.contains(parentScope));
                    parentScope = outEdges[parentScope];
                  }
                }
              }
            } else {
              NEXT = lIDX - 1;
            }
            REFIDX = lIDX++;
            lbAtScope[currScope].push_back(REFIDX);

            lb.setNEXT(NEXT);
            lb.setREFIDX(REFIDX);
            lBindings.push_back(lbID);
          }

          for (auto &rbID : RB) {
            RemoteEnvBindingSEXP rb(rbID, pool);
            rb.setREFIDX(rIDX++);
            if (rb.hasMODULE()) {
              EnvBindingSEXP eb(rb.getArg_ParentReference(), pool);
              eb.setREFIDX(rlIDX++);
            } else if (rb.hasMODULEI() || rb.hasMODULENSI()) {
              EnvBindingSEXP eb(rb.getArg_ParentReference(), pool);
              eb.setREFIDX(riIDX++);
            }
            rBindings.push_back(rbID);
          }

          // Recurse over children
          if (inEdges.contains(currScope)) {
            for (auto &c : inEdges[currScope]) {
              handleFrameBindings(c, false);
            }
          }
        };
    handleFrameBindings(currHead, true);

    BBContainerSupport bbc(bbcID, pool);

    BindingsSEXP bindings(bbc.getArg_Bindings(), pool);
    pool.set_args(bindings.getArg_LocalBindings(), lBindings);
    pool.set_args(bindings.getArg_RemoteBindings(), rBindings);
    for (size_t i = 0; i < cBindings.size(); i++) {
      PoolBindingSEXP pb(cBindings[i], pool);
      pb.setREFIDX(i);
    }
    pool.set_args(bindings.getArg_Lambdas(), cBindings);
  }
  // Resolve evals
  for (auto &[id, scopeToTaint, containerScope] : directEvals) {
    double evalREFIDX = getJSEvalLookupREFIDX(scopeToTaint, containerScope);
    if (pool[id].tag == IRI_GEN::CallSite) {
      CallSiteSEXP callSite(id, pool);
      callSite.setJSDirectEval(evalREFIDX);
    } else {
      ApplySEXP callSite(id, pool);
      callSite.setJSDirectEval(evalREFIDX);
    }
  }
}

bool IRIS::hasScopePath(double startScope, double targetScope,
                        bool breakAtClosureBoundary) {
  double currScope = startScope;
  while (currScope != -1) {
    if (breakAtClosureBoundary && scopeHead.contains(currScope))
      return false;
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
  for (auto &pis : propInitScopes) {
    if (hasScopePath(currScope, pis))
      return true;
  }
  return false;
}

double IRIS::getExceptionTargetForScope(double startScope) {
  double currScope = startScope;
  while (currScope != -1) {
    if (exceptionEdgeRedirect.contains(currScope)) {
      return exceptionEdgeRedirect[currScope];
    }

    if (scopeHead.contains(currScope))
      return -1;

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      assert(false && "Parent scope not found, error");
    }
    currScope = edgeIt->second;
  }
  return -1;
}

double IRIS::getFinalizerRetBBIDX(double finalizerIDX) {
  assert(finalizerRetMap.contains(finalizerIDX));
  return finalizerRetMap[finalizerIDX];
}

bool IRIS::isTopLevelScope(double startScope) {
  auto edgeIt = outEdges.find(startScope);
  if (edgeIt == outEdges.end()) {
    assert(false && "Parent scope not found, error");
  }
  return edgeIt->second == -1;
}

bool IRIS::mayReadFromATaintedScope(double startScope) {
  for (auto &ts : taintedScopes) {
    //
    // Does not cause a problem if the only reachable tainted scope is the
    // global scope itself
    //
    if (topLevelScope == ts)
      continue;
    if (hasScopePath(startScope, ts))
      return true;
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
    if (shadowedReads.contains(sid))
      continue;
    resolve(sid, startScope);
  }
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
        // This is a neat assertion...
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

std::vector<IRI_STORAGE::IRID> IRIS::getBindingsToMoveToHeap(double startScope,
                                                             double endScope) {
  if (!hasScopePath(startScope, endScope)) {
    return {};
  }

  if (startScope == endScope) {
    return {};
  }

  std::vector<IRI_STORAGE::IRID> res;

  double currScope = startScope;
  do {
    auto scopeIt = scopeBindings.find(currScope);
    if (scopeIt != scopeBindings.end()) {
      auto &bindingsMapAtScope = scopeIt->second;
      for (auto &b : bindingsMapAtScope) {
        auto bID = b.second;
        if (pool[bID].tag == IRI_GEN::EnvBinding && (*this)[bID].isCaptured()) {
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

void IRIS::dumpBindingsAtScope(std::ostream &oss, double currScope) const {
  if (!scopeBindings.contains(currScope)) {
    oss << "[No bindings]";
    return;
  }

  for (auto &[sID, bID] : scopeBindings.at(currScope)) {
    auto &meta = (*this)[bID];
    meta.dump(pool, oss, false);
    oss << " | ";
  }
}

void IRIS::printNode(
    std::ostream &oss, double node, std::string prefix, bool isLast,
    const std::unordered_map<double, std::vector<double>> &childrenMap) const {

  // Print current node with branching characters
  oss << prefix << (isLast ? "└── " : "├── ") << "("
      << (scopeHead.contains(node) ? "[*]" : "")
      << (argInitScopes.contains(node) ? "[A]" : "")
      << (propInitScopes.contains(node) ? "[P]" : "")
      << (tryScopes.contains(node) ? "[T]" : "")
      << (taintedScopes.contains(node) ? "[†]" : "") << ") " << node << " : ";
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

void IRIS::dumpFlat(std::ostream &oss, int indentLevel) const {
  oss << "\n=============================================\n";
  oss << "            IRIS STATE DUMP                  \n";
  oss << "=============================================\n";
  dumpScopeTree(oss, indentLevel);
  // std::cout << "All Declared Names" << std::endl;
  // for (auto & n : allNames) {
  //   std::cout << "  " << n << " : " << pool.strings.get(n) << std::endl;
  // }
  oss << "=============================================\n\n";
}

} // namespace IRI_STRUCTURAL
