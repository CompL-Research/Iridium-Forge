#include "Support/TryStructure.hpp"
#include "Parser/IridiumBuildContext.h"
#include "Support/IRIS.hpp"
#include <cassert>

namespace IRI_STRUCTURAL {

TryStructure::TryStructure(
    IRIS &iris,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iriBC)
    : iris(iris) {

  for (auto &[currScope, bcon] : iriBC) {
    if (!bcon->tryContext.has_value())
      continue;

    auto &lCon = bcon->tryContext.value();

    if (lCon.tryScopeIDX > -1) {
      BBIDX redirect = lCon.udCatchIDX > -1 ? (BBIDX)lCon.udCatchIDX
                                            : (BBIDX)lCon.imCatchIDX;
      assert(redirect != NO_BB);
      exceptionEdgeRedirect[lCon.tryScopeIDX] = redirect;
    }

    if (lCon.udCatchScopeIDX > -1) {
      assert(lCon.imCatchIDX != -1);
      exceptionEdgeRedirect[lCon.udCatchScopeIDX] = (BBIDX)lCon.imCatchIDX;
    }

    if (lCon.finalizerIDX > -1) {
      assert(lCon.finalizerRetIDX != -1);
      finalizerRetMap[(BBIDX)lCon.finalizerIDX] = (BBIDX)lCon.finalizerRetIDX;
    }
  }
}

BBIDX TryStructure::getExceptionTargetForScope(SCPIDX startScope) const {
  for (SCPIDX scope = startScope; scope != NO_SCOPE;
       scope = iris.parentScope(scope)) {
    auto it = exceptionEdgeRedirect.find(scope);
    if (it != exceptionEdgeRedirect.end())
      return it->second;
    if (iris.isContainerScope(scope))
      return NO_BB;
  }
  return NO_BB;
}

BBIDX TryStructure::getFinalizerRetBBIDX(BBIDX finalizerIDX) const {
  assert(finalizerRetMap.contains(finalizerIDX));
  return finalizerRetMap.at(finalizerIDX);
}

} // namespace IRI_STRUCTURAL
