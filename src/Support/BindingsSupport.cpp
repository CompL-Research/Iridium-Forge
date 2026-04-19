#include "Support/BindingsSupport.hpp"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/StringPool.h"
#include "Support/IndexedIterator.hpp"
#include <algorithm>
#include <functional>
#include <optional>
#include <stdexcept>

namespace IRI_STRUCTURAL {
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_PARSE;
using ITR_RET = IndexedIterator<std::vector<IRID>>;

ITR_RET BindingsSupport::localBindings() const {
  BindingsSEXP bSEXP(id, *pool);
  IRID bindingsID = bSEXP.getArg_LocalBindings();
  ListSEXP bindingsList(bindingsID, *pool);
  if (bindingsList.getTYPE() != pool->strings.intern("EnvBinding"))
    throw std::runtime_error(
        "Expected TYPE EnvBinding to be set on localBindings list");
  return IndexedIterator(std::move(pool->get_args(bindingsID)));
}

ITR_RET BindingsSupport::remoteBindings() const {
  BindingsSEXP bSEXP(id, *pool);
  IRID bindingsID = bSEXP.getArg_RemoteBindings();
  ListSEXP bindingsList(bindingsID, *pool);
  if (bindingsList.getTYPE() != pool->strings.intern("RemoteEnvBinding")) {
    throw std::runtime_error("Expected TYPE RemoteEnvBinding to be set on "
                             "remoteBindings list, found: ");
  }
  return IndexedIterator(std::move(pool->get_args(bindingsID)));
}

ITR_RET BindingsSupport::lambdas() const {
  BindingsSEXP bSEXP(id, *pool);
  IRID bindingsID = bSEXP.getArg_Lambdas();
  ListSEXP bindingsList(bindingsID, *pool);
  if (bindingsList.getTYPE() != pool->strings.intern("PoolBinding"))
    throw std::runtime_error(
        "Expected TYPE PoolBinding to be set on poolBindings list");
  return IndexedIterator(std::move(pool->get_args(bindingsID)));
}
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void BindingsSupport::balance(BUILD_CTX &buildCTX) {

  bool isTopLevel = getParentScope() == -1;

  // 1. Process remote bindings

  int remoteRefIdx = 0;
  for (auto [remoteIrid, _] : remoteBindings()) {
    IRI_GEN::RemoteEnvBindingSEXP remoteBinding(remoteIrid, *pool);
    int currentIdx = remoteRefIdx++;
    remoteBinding.setREFIDX(currentIdx);

    //
    // For modules, top level remote bindings are exported bindings
    // they share the same offset on the stack.
    //
    if (isTopLevel) {
      IRID resolved = IRI_HELPERS::resolveRemoteBinding(*pool, remoteIrid);
      IRI_GEN::EnvBindingSEXP parentBinding(resolved, *pool);
      parentBinding.setREFIDX(currentIdx);
    }
  }

  // 2. Separate Local Bindings: Function args vs. Scope-mapped bindings
  std::vector<IRI_GEN::IRID> funcArgs;
  std::unordered_map<double, std::vector<IRI_GEN::IRID>> mapping;

  for (auto [localIrid, _] : localBindings()) {
    IRI_GEN::EnvBindingSEXP binding(localIrid, *pool);

    if (binding.hasJSARG() || binding.hasJSRESTARG()) {
      funcArgs.push_back(localIrid);
    } else {
      mapping[binding.getScope()].push_back(localIrid);
    }
  }

  // 3. Sort mapped bindings by scope ID and flatten them
  std::vector<std::pair<double, std::vector<IRI_GEN::IRID>>> sortedMappings(
      mapping.begin(), mapping.end());
  std::sort(sortedMappings.begin(), sortedMappings.end(),
            [](const auto &a, const auto &b) { return a.first < b.first; });

  std::vector<IRI_GEN::IRID> flattened;
  std::unordered_map<double, int> lastRefIdxPerScope;
  int localRefIdx = 0;

  for (const auto &[scope, vec] : sortedMappings) {
    bool isFirst = true;

    for (IRI_GEN::IRID bindingIrid : vec) {
      IRI_GEN::EnvBindingSEXP binding(bindingIrid, *pool);
      int currentIdx = localRefIdx++;

      binding.setREFIDX(currentIdx);
      binding.setNEXT(isFirst ? -1 : currentIdx - 1);

      flattened.push_back(bindingIrid);
      lastRefIdxPerScope[scope] = currentIdx;

      isFirst = false;
    }
  }

  // 4. Patch NEXT pointers for the head of each scope
  std::function<int(double)> patchToParentIDX = [&](double scopeIdx) -> int {
    if (scopeIdx == -1)
      return -1;

    if (auto it = lastRefIdxPerScope.find(scopeIdx);
        it != lastRefIdxPerScope.end()) {
      return it->second;
    }

    double parentScopeIdx = buildCTX[scopeIdx]->parent;
    return patchToParentIDX(parentScopeIdx);
  };

  for (IRI_GEN::IRID flattenedIrid : flattened) {
    IRI_GEN::EnvBindingSEXP binding(flattenedIrid, *pool);
    if (binding.getNEXT() == -1) {
      binding.setNEXT(patchToParentIDX(binding.getParentScope()));
    }
  }

  // 5. Rebuild and commit the Local Bindings array
  std::vector<IRI_GEN::IRID> converted;
  converted.reserve(funcArgs.size() + flattened.size());

  // Batch inserts are faster and cleaner than manual push_back loops
  converted.insert(converted.end(), funcArgs.begin(), funcArgs.end());
  converted.insert(converted.end(), flattened.begin(), flattened.end());

  pool->set_args(getArg_LocalBindings(), converted);
}

std::optional<IRID> BindingsSupport::getBinding(BUILD_CTX &iridiumBuildContext,
                                                StringID name,
                                                double lookupScope) {
  if (lookupScope == -1)
    return std::nullopt
    ;
  for (auto [bID, _] : localBindings()) {
    EnvBindingSEXP bSEXP(bID, *pool);
    if (bSEXP.getScope() == lookupScope && bSEXP.getNAME() == name)
      return bID;
  }

  for (auto [rbID, _] : remoteBindings()) {
    RemoteEnvBindingSEXP rbSEXP(rbID, *pool);
    auto bID = IRI_HELPERS::resolveRemoteBinding(*pool, rbID);
    EnvBindingSEXP bSEXP(bID, *pool);
    if (bSEXP.getScope() == lookupScope && bSEXP.getNAME() == name)
      return rbID;
  }

  auto &buildContext = iridiumBuildContext[lookupScope];
  double nextScope = buildContext->parent;
  // If the scope is an ArgInit context, bypass lookup of non-argument bindings
  // to parent scope
  if (buildContext->isArgInitContext) {
    // If the binding is not in the whitelist, bypass lookup scope...
    if (buildContext->argInitContextWhitelist.find(std::string(pool->strings.get(name))) ==
        buildContext->argInitContextWhitelist.end()) {
      nextScope = buildContext->bypassParent;
    }
  }
  return getBinding(iridiumBuildContext, name, nextScope);
}

} // namespace IRI_STRUCTURAL
