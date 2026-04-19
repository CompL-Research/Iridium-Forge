
#pragma once
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/StringPool.h"
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

  bool isGlobal(StringID, double);

  IRI_STORAGE::IRID resolve(StringID, double);

  void commit();

private:
  // Node Storage Pool
  IRI_STORAGE::IridiumPool &pool;

  // Graph
  // In a scope tree, each node has atmost one outgoing edges, with -1 denoting
  // end of top level scope
  std::unordered_map<double, double> outEdges;
  std::set<double> nodes;

  // Globals lookup fastcase, if a StringID is never encountered, it never
  // existed.
  std::set<StringID> allNames;

  // Scopes belonging to a BBContainer
  std::unordered_map<double, IRI_GEN::IRID> scopeHead;

  // At a given scope O(1) check if a StringID exists
  std::unordered_map<double, std::unordered_map<StringID, IRI_STORAGE::IRID>>
      scopeBindings;

  // Commit list
  // For a given scope, commit newly created remote bindings at the very end
  // to avoid repeated resizing of the pool
  std::unordered_map<double, std::vector<IRI_STORAGE::IRID>> commitList;
public:
  void dumpScopeTree() const;
  void dumpBindings() const;
  void dumpCommitList() const;
  void dumpAllNames() const;
  void dumpFullState() const;
};
} // namespace IRI_STRUCTURAL
