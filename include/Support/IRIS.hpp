
#pragma once
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/BindingsPool.h"
#include "Storage/Config.h"
#include "Storage/StringPool.h"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_STRUCTURAL {

class IRIS {
public:
  IRIS(IRI_STORAGE::IridiumPool &,
       std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
           &,
       IRI_GEN::IRID);

  // Scope Related
  double getTopLevelScope() { return topLevelScope; }

  // Binding Declaration
  IRI_STORAGE::BindingMeta & declareScriptBinding(double, StringID, IRI_GEN::IRI_FLAG);
  IRI_STORAGE::BindingMeta & declareLBinding(double, StringID, IRI_GEN::IRI_FLAG);
  IRI_STORAGE::BindingMeta & declareRBinding(double, StringID, IRI_GEN::IRI_FLAG, IRI_GEN::IRI_FLAG);
  void addClosureAtScope(double, IRI_STORAGE::IRID);

  bool isGlobal(StringID sid, double startScopeIDX);

  IRI_STORAGE::BindingMeta &operator[](IRI_STORAGE::IRID bID) {
    return bindingsPool->getMetaFromIRID(pool, bID);
  }

  const IRI_STORAGE::BindingMeta &operator[](IRI_STORAGE::IRID bID) const {
    return bindingsPool->getMetaFromIRID(pool, bID);
  }

  IRI_STORAGE::BindingMeta &getBindingMetaFromLINK(double LINK) {
    return (*bindingsPool)[LINK];
  }

  const IRI_STORAGE::BindingMeta &getBindingMetaFromLINK(double LINK) const {
    return (*bindingsPool)[LINK];
  }

  bool hasBinding(StringID, double);
  void removeBinding(StringID, double);

  double getLexicalScope(double);
  IRI_STORAGE::BindingMeta & resolve(StringID, double);

  void registerDirectEval(IRI_STORAGE::IRID, double, double);

  void commit();

  bool hasScopePath(double, double,  bool breakAtClosureBoundary = false);

  bool isTopLevelScope(double);

  bool mayReadFromATaintedScope(double);

  void addEvalRemoteBindingsToParentClosure(double);

  double getJSEvalLookupREFIDX(double, double);

  double getEnclosingThrowScope(double);

  double getEnclosingClosureScope(double);

  double isArgInitScope(double);

  bool isEnclosedInAPropInitScope(double);

  double getExceptionTargetForScope(double);

  double getFinalizerRetBBIDX(double);

  size_t computePoolCapacity();

  void populateCClosuresInTree();

  std::vector<IRI_STORAGE::IRID> getBindingsToMoveToHeap(double startScope, double endScope);

  void ensureExportedBindingIsModuleBinding(StringID id) {
    exportedModuleBindings.insert(id);
  }

  bool isExportedBinding(StringID id) {
    return exportedModuleBindings.contains(id);
  }

private:
  // Node Storage Pool
  IRI_STORAGE::IridiumPool &pool;
  // Bindings Metadata Pool
  std::unique_ptr<IRI_STORAGE::BindingsPool> bindingsPool = std::make_unique<IRI_STORAGE::BindingsPool>();

  // Graph
  // In a scope tree, each node has atmost one outgoing edges, with -1 denoting
  // end of top level scope
  std::unordered_map<double, double> outEdges;
  std::unordered_map<double, std::set<double>> inEdges;
  std::set<double> nodes;

  std::vector<std::tuple<IRI_STORAGE::IRID, double, double>> directEvals;

  double topLevelScope = -1;

  std::set<StringID> allNames;

  // Special Nodes
  std::unordered_map<double, IRI_GEN::IRID> scopeHead;
  std::set<double> argInitScopes;
  std::set<double> propInitScopes;
  std::set<double> taintedScopes;
  std::set<double> tryScopes;
  std::unordered_map<double, std::vector<IRI_STORAGE::IRID>> argsAtScope;

  // At a given scope O(1) check if a StringID exists
  std::unordered_map<double, std::unordered_map<StringID, IRI_STORAGE::IRID>>
      scopeBindings;

  std::unordered_map<StringID, IRI_STORAGE::IRID> scriptBindings;
  std::unordered_map<StringID, IRI_STORAGE::IRID> globalBindings;
  std::unordered_map<double, std::vector<IRI_STORAGE::IRID>> closuresAtScope;

  std::unordered_map<double, double> exceptionEdgeRedirect;
  std::unordered_map<double, double> finalizerRetMap;

  std::set<StringID> exportedModuleBindings;

  void printNode(
      std::ostream &oss, double node, std::string prefix, bool isLast,
      const std::unordered_map<double, std::vector<double>> &childrenMap) const;

public:
  void dumpFlat(std::ostream &oss, int indentLevel = 0) const;
  void dumpScopeTree(std::ostream &oss, int indentLevel = 0) const;
  void dumpBindingsAtScope(std::ostream &oss, double node) const;
};
} // namespace IRI_STRUCTURAL
