#pragma once
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/BindingsPool.h"
#include "Storage/Config.h"
#include "Storage/StringPool.h"
#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_STRUCTURAL {

using namespace IRI_STORAGE;

class IRIS {
public:
  IRIS(IRI_STORAGE::IRIContext &,
       std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
           &,
       IRI_GEN::IRID);

  // Scope Related
  SCPIDX getTopLevelScope() { return topLevelScope; }

  SCPIDX parentScope(SCPIDX scope) const {
    auto it = outEdges.find(scope);
    return it == outEdges.end() ? NO_SCOPE : it->second;
  }
  IRI_GEN::IRID containerForScope(SCPIDX scope) const {
    return scopeHead.at(scope);
  }
  bool isContainerScope(SCPIDX scope) const {
    return scopeHead.contains(scope);
  }

  // Binding Declaration
  IRI_STORAGE::BindingMeta &declareScriptBinding(SCPIDX, StringID,
                                                 IRI_GEN::IRI_FLAG);
  IRI_STORAGE::BindingMeta &declareLBinding(SCPIDX, StringID,
                                            IRI_GEN::IRI_FLAG);
  IRI_STORAGE::BindingMeta &declareRBinding(SCPIDX, StringID,
                                            IRI_GEN::IRI_FLAG,
                                            IRI_GEN::IRI_FLAG);
  bool isGlobal(StringID sid, SCPIDX startScopeIDX);

  IRI_STORAGE::BindingMeta &get_binding(IRI_STORAGE::IRID bID) {
    return ctx.storage.bindings.getMetaFromIRID(ctx, bID);
  }

  const IRI_STORAGE::BindingMeta &get_binding(IRI_STORAGE::IRID bID) const {
    return ctx.storage.bindings.getMetaFromIRID(ctx, bID);
  }

  IRI_STORAGE::BindingMeta &getBindingMetaFromLINK(uint32_t LINK) {
    return ctx.storage.bindings.get(LINK);
  }

  const IRI_STORAGE::BindingMeta &getBindingMetaFromLINK(uint32_t LINK) const {
    return ctx.storage.bindings.get(LINK);
  }

  bool hasBinding(StringID, SCPIDX);
  void removeBinding(StringID, SCPIDX);

  SCPIDX getLexicalScope(SCPIDX);
  IRI_STORAGE::BindingMeta &resolve(StringID, SCPIDX);

  void registerDirectEval(IRI_STORAGE::IRID, SCPIDX, SCPIDX);

  void commit();

  bool hasScopePath(SCPIDX, SCPIDX, bool breakAtClosureBoundary = false);

  bool isTopLevelScope(SCPIDX);

  bool mayReadFromATaintedScope(SCPIDX);

  void addEvalRemoteBindingsToParentClosure(SCPIDX);

  SCPIDX getJSEvalLookupREFIDX(SCPIDX, SCPIDX);

  SCPIDX getEnclosingThrowScope(SCPIDX);

  SCPIDX getEnclosingClosureScope(SCPIDX) const;

  bool isArgInitScope(SCPIDX);

  bool isEnclosedInAPropInitScope(SCPIDX);

  std::vector<IRI_STORAGE::IRID> getBindingsToMoveToHeap(SCPIDX startScope,
                                                         SCPIDX endScope);

  std::vector<IRI_STORAGE::IRID> getBindingsInClosure(SCPIDX headScope);

  std::vector<IRI_STORAGE::IRID> getEnvBindingsAtScope(SCPIDX scope);
  std::vector<IRI_STORAGE::IRID> getRemoteEnvBindingsAtScope(SCPIDX scope);

  std::vector<IRI_STORAGE::IRID> getEnvBindingsInClosure(SCPIDX headScope);
  std::vector<IRI_STORAGE::IRID>
  getRemoteEnvBindingsInClosure(SCPIDX headScope);

  void ensureExportedBindingIsModuleBinding(StringID id) {
    exportedModuleBindings.insert(id);
  }

  bool isExportedBinding(StringID id) {
    return exportedModuleBindings.contains(id);
  }

  // IRIS can declare bindings during core passes
  // which are not connected by args of any node until iris commit
  // Add those reference IRIDs into the root set to include in reachability result
  // so that iris->remapIrids don't see these IRIDs as garbage
  std::vector<IRI_STORAGE::IRID> collectReferencedIrids() const;

  // Remap IRID references after compaction by GC
  // IRIDs in scopeHead, argsAtScope, scopeBindings, scriptBindings,
  // globalBindings, directEvals
  // bindingsPool -> handled by BindingsPool::remapIrids
  void remapIrids(std::span<const IRI_STORAGE::IRID> old_to_new_irid_map);

  std::vector<IRI_STORAGE::IRID> getScriptBindings() {
    std::vector<IRI_STORAGE::IRID> res;
    res.reserve(scriptBindings.size());
    std::transform(scriptBindings.begin(), scriptBindings.end(),
                   std::back_inserter(res),
                   [](const auto e) { return e.second; });
    return res;
  }

  std::vector<IRI_STORAGE::IRID> getGlobalBindings() {
    std::vector<IRI_STORAGE::IRID> res;
    res.reserve(globalBindings.size());
    std::transform(globalBindings.begin(), globalBindings.end(),
                   std::back_inserter(res),
                   [](const auto e) { return e.second; });
    return res;
  }

  // The argument bindings (JSARG / JSRESTARG) declared at a closure's head
  // scope, before commit() lays out the frame; null if there are none.
  const std::vector<IRI_STORAGE::IRID> *getArgsAtScope(SCPIDX scope) const {
    auto it = argsAtScope.find(scope);
    return it == argsAtScope.end() ? nullptr : &it->second;
  }

  void dumpFlat(std::ostream &oss, int indentLevel = 0) const;
  void dumpScopeTree(std::ostream &oss, int indentLevel = 0) const;
  void dumpBindingsAtScope(std::ostream &oss, SCPIDX node) const;

private:
  // Node Storage Pool
  IRI_STORAGE::IRIContext &ctx;

  // Scope tree. Each node has at most one out-edge, NO_SCOPE ends the chain.
  std::unordered_map<SCPIDX, SCPIDX> outEdges;
  std::unordered_map<SCPIDX, std::set<SCPIDX>> inEdges;
  std::set<SCPIDX> nodes;

  std::vector<std::tuple<IRI_STORAGE::IRID, SCPIDX, SCPIDX>> directEvals;

  SCPIDX topLevelScope = NO_SCOPE;

  std::set<StringID> allNames;

  // Special Nodes
  std::unordered_map<SCPIDX, IRI_GEN::IRID> scopeHead;
  std::set<SCPIDX> argInitScopes;
  std::set<SCPIDX> propInitScopes;
  std::set<SCPIDX> taintedScopes;
  std::set<SCPIDX> tryScopes;
  std::unordered_map<SCPIDX, std::vector<IRI_STORAGE::IRID>> argsAtScope;

  // At a given scope O(1) check if a StringID exists
  std::unordered_map<SCPIDX, std::unordered_map<StringID, IRI_STORAGE::IRID>>
      scopeBindings;

  std::unordered_map<StringID, IRI_STORAGE::IRID> scriptBindings;
  std::unordered_map<StringID, IRI_STORAGE::IRID> globalBindings;

  std::set<StringID> exportedModuleBindings;

  void printNode(
      std::ostream &oss, SCPIDX node, std::string prefix, bool isLast,
      const std::unordered_map<SCPIDX, std::vector<SCPIDX>> &childrenMap)
      const;
};
} // namespace IRI_STRUCTURAL

// Readability shorthand for IRIS::get_binding, replacing the old
// (*iris)[id] operator[] syntax.
#define IRI_BINDING(iris, id) ((iris)->get_binding(id))
