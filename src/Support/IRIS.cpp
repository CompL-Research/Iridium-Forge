#include "Support/IRIS.hpp"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"
#include <iomanip>
#include <stdexcept>
#include <vector>

// Toggle this to 'true' to enable execution tracing
constexpr bool TRACE_IRIS_LOGIC = false;

#define IRIS_TRACE(msg)                                                        \
  if constexpr (TRACE_IRIS_LOGIC) {                                            \
    std::cerr << "[IRIS TRACE] " << msg << "\n";                               \
  }

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
    auto curr = e.first;
    auto parent = e.second->parent;
    outEdges[curr] = parent;
    nodes.insert(curr);
    nodes.insert(parent);
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
      allNames.insert(b.getNAME());
      scopeBindings[containerScope][b.getNAME()] = rbID;
    }
  }
}

// bool IRIS::isGlobal(StringID sid, double startScopeIDX) {
//   if (!allNames.contains(sid))
//     return true;

//   double currScope = startScopeIDX;
//   while (currScope != -1) {
//     auto scopeIt = scopeBindings.find(currScope);
//     if (scopeIt != scopeBindings.end() && scopeIt->second.contains(sid)) {
//       return false;
//     }

//     auto edgeIt = outEdges.find(currScope);
//     if (edgeIt == outEdges.end()) {
//       throw std::runtime_error("Parent scope not found, error");
//     }
//     currScope = edgeIt->second;
//   }

//   return true;
// }
//
bool IRIS::isGlobal(StringID sid, double startScopeIDX) {
  if constexpr (TRACE_IRIS_LOGIC) {
    std::cerr << "[IRIS TRACE] isGlobal check -> Name: '"
              << pool.strings.get(sid) << "' (ID: " << sid
              << ") starting at Scope: " << startScopeIDX << "\n";
  }

  if (!allNames.contains(sid)) {
    IRIS_TRACE("Name not in allNames. Returning true (is global).");
    return true;
  }

  double currScope = startScopeIDX;
  while (currScope != -1) {
    IRIS_TRACE("  Visiting Scope: " << currScope);

    auto scopeIt = scopeBindings.find(currScope);
    if (scopeIt != scopeBindings.end() && scopeIt->second.contains(sid)) {
      IRIS_TRACE("  Found local binding in Scope: "
                 << currScope << ". Returning false (not global).");
      return false;
    }

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      throw std::runtime_error("Parent scope not found, error");
    }
    currScope = edgeIt->second;
  }

  IRIS_TRACE(
      "Reached top-level scope without finding local binding. Returning true.");
  return true;
}

// IRI_STORAGE::IRID IRIS::resolve(StringID sid, double startScopeIDX) {

//   std::vector<double> headsCrossed;

//   double currScope = startScopeIDX;
//   IRI_STORAGE::IRID found;
//   while (true) {

//     // Safe lookup: avoids inserting empty maps into scopeBindings
//     auto scopeIt = scopeBindings.find(currScope);
//     if (scopeIt != scopeBindings.end() && scopeIt->second.contains(sid)) {
//       found = scopeIt->second.at(sid);
//       break;
//     }

//     if (scopeHead.contains(currScope))
//       headsCrossed.push_back(currScope);

//     auto edgeIt = outEdges.find(currScope);
//     if (edgeIt == outEdges.end()) {
//       throw std::runtime_error(
//           "Failed to resolve env binding: scope chain broken");
//     }
//     currScope = edgeIt->second;

//     if (currScope == -1) {
//       throw std::runtime_error(
//           "Failed to resolve env binding: reached top-level scope");
//     }
//   }

//   if (headsCrossed.size() > 0) {
//     while (!headsCrossed.empty()) {
//       auto currHead = headsCrossed.back();
//       headsCrossed.pop_back();
//       found = IRI_GEN::RemoteEnvBindingSEXP::create(pool, found, false, -1);
//       commitList[currHead].push_back(found);
//       scopeBindings[currHead][sid] = found;
//     }
//   }

//   return found;
// }

IRI_STORAGE::IRID IRIS::resolve(StringID sid, double startScopeIDX) {
  if constexpr (TRACE_IRIS_LOGIC) {
    std::cerr << "[IRIS TRACE] resolve requested -> Name: '"
              << pool.strings.get(sid) << "' (ID: " << sid
              << ") starting at Scope: " << startScopeIDX << "\n";
  }

  std::vector<double> headsCrossed;
  double currScope = startScopeIDX;
  IRI_STORAGE::IRID found;

  while (true) {
    IRIS_TRACE("  Searching in Scope: " << currScope);

    auto scopeIt = scopeBindings.find(currScope);
    if (scopeIt != scopeBindings.end() && scopeIt->second.contains(sid)) {
      found = scopeIt->second.at(sid);
      IRIS_TRACE("  Found existing IRID: " << found
                                           << " in Scope: " << currScope);
      break;
    }

    if (scopeHead.contains(currScope)) {
      IRIS_TRACE("  Crossed BBContainer boundary at head Scope: " << currScope);
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
    IRIS_TRACE("  Wiring " << headsCrossed.size()
                           << " remote binding(s) across boundaries...");
    while (!headsCrossed.empty()) {
      auto currHead = headsCrossed.back();
      headsCrossed.pop_back();
      found = IRI_GEN::RemoteEnvBindingSEXP::create(pool, found, false, -1);

      IRIS_TRACE("    Created RemoteEnvBinding IRID: "
                 << found << " for head Scope: " << currHead);

      commitList[currHead].push_back(found);
      scopeBindings[currHead][sid] = found;
    }
  }

  IRIS_TRACE("Resolution complete. Returning IRID: " << found);
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
}

bool IRIS::hasScopePath(double startScope, double targetScope) {
  double currScope = startScope;
  while (currScope != -1) {
    if (currScope == targetScope)
      return true;

    auto edgeIt = outEdges.find(currScope);
    if (edgeIt == outEdges.end()) {
      throw std::runtime_error("Parent scope not found, error");
    }
    currScope = edgeIt->second;
  }
  return false;
}

void IRIS::dumpScopeTree() const {
  std::cerr << "=== IRIS Scope Tree (Current -> Parent) ===\n";
  if (outEdges.empty()) {
    std::cerr << "  [Empty]\n";
    return;
  }
  for (const auto &[curr, parent] : outEdges) {
    std::cerr << "  Scope " << std::left << std::setw(6) << curr << " -> "
              << parent << "\n";
  }
}

void IRIS::dumpCommitList() const {
  std::cerr << "=== IRIS Pending Commit List ===\n";
  if (commitList.empty()) {
    std::cerr << "  [Empty]\n";
    return;
  }
  for (const auto &[headScope, commits] : commitList) {
    std::cerr << "  BBContainer Head Scope " << headScope << " ("
              << commits.size() << " pending remote bindings):\n";
    for (size_t i = 0; i < commits.size(); ++i) {
      std::cerr << "    [" << i << "] IRID: " << commits[i] << "\n";
    }
  }
}

void IRIS::dumpBindings() const {
  std::cerr << "=== IRIS Scope Bindings ===\n";
  if (scopeBindings.empty()) {
    std::cerr << "  [Empty]\n";
    return;
  }
  for (const auto &[scope, bindings] : scopeBindings) {
    std::cerr << "  Scope " << scope << ":\n";
    if (bindings.empty()) {
      std::cerr << "    [No bindings]\n";
    } else {
      for (const auto &[nameID, irid] : bindings) {
        // Resolving the StringID via the pool
        std::cerr << "    Name: " << std::left << std::setw(15)
                  << pool.strings.get(nameID) << " (ID: " << nameID
                  << ") -> IRID: " << irid << "\n";
      }
    }
  }
}

void IRIS::dumpAllNames() const {
  std::cerr << "=== IRIS Fast-Lookup Global Names ===\n";
  if (allNames.empty()) {
    std::cerr << "  [Empty]\n";
    return;
  }
  std::cerr << "  ";
  for (const auto &nameID : allNames) {
    // Resolving the StringID via the pool
    std::cerr << pool.strings.get(nameID) << "[" << nameID << "]" << " ";
  }
  std::cerr << "\n";
}

void IRIS::dumpFullState() const {
  std::cerr << "\n=============================================\n";
  std::cerr << "            IRIS STATE DUMP                  \n";
  std::cerr << "=============================================\n";
  dumpAllNames();
  dumpScopeTree();
  dumpBindings();
  dumpCommitList();
  std::cerr << "=============================================\n\n";
}

} // namespace IRI_STRUCTURAL
