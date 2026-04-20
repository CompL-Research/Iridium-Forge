#include "Helpers.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Storage/StringPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"
#include <functional>
#include <stdexcept>

namespace IRI_HELPERS {

auto findVARHoistingScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) -> double {
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " +
                             std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  IRI_GEN::BBSEXP startBB(buildContext->BB[0], pool);
  if (startBB.hasClosureBoundary() || startBB.hasTopLevel() ||
      startBB.hasVARBoundary())
    return startingScope;
  return findVARHoistingScope(pool, buildContext->parent, iridiumBuildContext);
}

auto findParentClosureScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) -> double {
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " +
                             std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  IRI_GEN::BBSEXP startBB(buildContext->BB.at(0), pool);

  if (startBB.hasClosureBoundary() || startBB.hasTopLevel())
    return startingScope;
  return findParentClosureScope(pool, buildContext->parent,
                                iridiumBuildContext);
}

auto getLexicalScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) -> double {
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " +
                             std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  return buildContext->parent;
}

IRI_STORAGE::IRID getTopLevelContainer(IRI_STORAGE::IridiumPool &pool,
                                       IRI_STORAGE::IRID fileSEXP) {
  if (pool.topLevelBBContainer.has_value())
    return pool.topLevelBBContainer.value();
  for (auto &bbcIDX : pool.get_args(fileSEXP)) {
    if (pool[bbcIDX].tag != IRI_GEN::BBContainer)
      continue;

    auto container = IRI_GEN::BBContainerSEXP(bbcIDX, pool);
    if (container.hasTopLevel()) {
      pool.topLevelBBContainer = bbcIDX;
      return bbcIDX;
    }
  }
  throw std::runtime_error("[Forge] Failed to find top level container SEXP");
}
IRI_STORAGE::IRID resolveRemoteBinding(IRI_STORAGE::IridiumPool &pool,
                                       IRI_STORAGE::IRID rbinID) {
  IRI_GEN::RemoteEnvBindingSEXP rbin(rbinID, pool);
  IRI_STORAGE::IRID containedBinding = rbin.getArg_ParentReference();
  if (pool[containedBinding].tag == IRI_GEN::EnvBinding)
    return containedBinding;
  else
    return resolveRemoteBinding(pool, containedBinding);
}

double getTopLevelScope(IRI_STORAGE::IridiumPool &pool,
                        IRI_STORAGE::IRID fileSEXP) {
  IRI_GEN::IRID topLevelContainer = getTopLevelContainer(pool, fileSEXP);
  IRI_GEN::BBContainerSEXP c(topLevelContainer, pool);

  return c.getScopeIDX();
}

void countNodeOccurenceWithPredicate(
    IRI_STORAGE::IRID id, IRI_STORAGE::IridiumPool *pool,
    std::function<bool(IRI_STORAGE::IRID)> pred, size_t &count) {

  if (pred(id))
    count++;
  for (auto &i : pool->get_args_view(id))
    countNodeOccurenceWithPredicate(i, pool, pred, count);
}

bool hasNodeWithPredicate(IRI_GEN::IRID id, IRI_GEN::IridiumPool *pool,
                          std::function<bool(IRI_STORAGE::IRID)> pred) {

  if (pred(id))
    return true;
  for (auto &i : pool->get_args_view(id))
    if (hasNodeWithPredicate(i, pool, pred))
      return true;

  return false;
}

IRI_STORAGE::IRID
createNoASWResolveEnvBindingSEXP(IRI_STORAGE::IridiumPool &pool, StringID s) {
  return IRI_GEN::ResolveEnvBindingSEXP::create(pool, s, false);
}

IRI_STORAGE::IRID createUnsafeEnvReadSEXP(IRI_STORAGE::IridiumPool &pool,
                                          StringID s) {
  return IRI_GEN::EnvReadSEXP::create(
      pool, createNoASWResolveEnvBindingSEXP(pool, s), false);
}

namespace {
    // 1. Shared Cache Key
    struct BindingKey {
        StringID name;
        double startScope;
        IRI_GEN::IRID bindingsID; // The unique ID of the bindings object in the pool

        bool operator==(const BindingKey& other) const {
            return name == other.name &&
                   startScope == other.startScope &&
                   bindingsID == other.bindingsID;
        }
    };

    // 2. Shared Hasher
    struct BindingKeyHash {
        std::size_t operator()(const BindingKey& k) const {
            std::size_t h = std::hash<int>{}(k.name);
            h ^= std::hash<double>{}(k.startScope) + 0x9e3779b9 + (h << 6) + (h >> 2);
            h ^= std::hash<int>{}(k.bindingsID) + 0x9e3779b9 + (h << 6) + (h >> 2);
            return h;
        }
    };

    // 3. The Caches
    std::unordered_map<BindingKey, bool, BindingKeyHash> g_isGlobalCache;
    std::unordered_map<BindingKey, IRI_GEN::IRID, BindingKeyHash> g_resolveLookupCache;
}

bool isGlobalBinding(
    IRI_STORAGE::IridiumPool &pool, IRI_STRUCTURAL::FileSupport fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>> &iridiumBuildContext,
    StringID name, double startScope,
    IRI_STRUCTURAL::BindingsSupport bindingsSEXP)
{
    // NOTE: Replace .getID() with however you access the underlying IRID in BindingsSupport
    BindingKey key{name, startScope, bindingsSEXP.id};

    auto it = g_isGlobalCache.find(key);
    if (it != g_isGlobalCache.end()) {
        return it->second;
    }

    auto res = bindingsSEXP.getBinding(iridiumBuildContext, name, startScope);
    if (res.has_value()) {
        return g_isGlobalCache[key] = false;
    }

    auto parentScope = bindingsSEXP.getParentScope();
    if (parentScope == -1) {
        return g_isGlobalCache[key] = true;
    }

    auto bbContainerID = fileSEXP.getBBContainerByScopeIDX(findParentClosureScope(pool, parentScope, iridiumBuildContext));
    IRI_STRUCTURAL::BBContainerSupport bbContainer(bbContainerID, pool);
    IRI_STRUCTURAL::BindingsSupport next(bbContainer.getArg_Bindings(), pool);

    return g_isGlobalCache[key] = isGlobalBinding(pool, fileSEXP, iridiumBuildContext, name, startScope, next);
}

// --- Cached resolveScopedLookup ---
IRI_GEN::IRID resolveScopedLookup(
    IRI_STORAGE::IridiumPool &pool, IRI_STRUCTURAL::FileSupport fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>> &iridiumBuildContext,
    StringID name, double startScope,
    IRI_STRUCTURAL::BindingsSupport bindingsSEXP)
{
    BindingKey key{name, startScope, bindingsSEXP.id};

    auto it = g_resolveLookupCache.find(key);
    if (it != g_resolveLookupCache.end()) {
        return it->second;
    }

    auto res = bindingsSEXP.getBinding(iridiumBuildContext, name, startScope);
    if (res.has_value()) {
        return g_resolveLookupCache[key] = res.value();
    }

    auto parentScope = bindingsSEXP.getParentScope();
    if (parentScope == -1) {
        throw std::runtime_error("Failed to resolve lookup: " + std::string(pool.strings.get(name)));
    }

    auto bbContainerID = fileSEXP.getBBContainerByScopeIDX(findParentClosureScope(pool, parentScope, iridiumBuildContext));
    IRI_STRUCTURAL::BBContainerSupport bbContainer(bbContainerID, pool);
    IRI_STRUCTURAL::BindingsSupport next(bbContainer.getArg_Bindings(), pool);

    IRI_GEN::IRID resolvedInParent = resolveScopedLookup(pool, fileSEXP, iridiumBuildContext, name, startScope, next);

    IRI_GEN::ListSEXP remoteBindingsList(bindingsSEXP.getArg_RemoteBindings(), pool);
    auto updatedRemoteBindings = pool.get_args(remoteBindingsList.id);

    IRI_GEN::IRID newlyCreatedRemoteBindingSEXP = IRI_GEN::RemoteEnvBindingSEXP::create(
        pool,
        resolvedInParent,
        false,
        static_cast<double>(updatedRemoteBindings.size())
    );

    updatedRemoteBindings.push_back(newlyCreatedRemoteBindingSEXP);
    pool.set_args(remoteBindingsList.id, updatedRemoteBindings);

    return g_resolveLookupCache[key] = newlyCreatedRemoteBindingSEXP;
}

// bool isGlobalBinding(
//     IRI_STORAGE::IridiumPool &pool, IRI_STRUCTURAL::FileSupport fileSEXP,
//     std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
//         &iridiumBuildContext,
//     StringID name, double startScope,
//     IRI_STRUCTURAL::BindingsSupport bindingsSEXP) {
//   auto res = bindingsSEXP.getBinding(iridiumBuildContext, name, startScope);
//   if (res.has_value())
//     return false;
//   auto parentScope = bindingsSEXP.getParentScope();
//   if (parentScope == -1)
//     return true;
//   auto bbContainerID =
//       fileSEXP[findParentClosureScope(pool, parentScope, iridiumBuildContext)];
//   IRI_STRUCTURAL::BBContainerSupport bbContainer(bbContainerID, pool);
//   IRI_STRUCTURAL::BindingsSupport next(bbContainer.getArg_Bindings(), pool);

//   return isGlobalBinding(pool, fileSEXP, iridiumBuildContext, name, startScope,
//                          next);
// }

// IRI_GEN::IRID resolveScopedLookup(
//     IRI_STORAGE::IridiumPool &pool, IRI_STRUCTURAL::FileSupport fileSEXP,
//     std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
//         &iridiumBuildContext,
//     StringID name, double startScope,
//     IRI_STRUCTURAL::BindingsSupport bindingsSEXP) {
//   auto res = bindingsSEXP.getBinding(iridiumBuildContext, name, startScope);
//   if (res.has_value())
//     return res.value();
//   auto parentScope = bindingsSEXP.getParentScope();
//   if (parentScope == -1)
//     throw std::runtime_error("Failed to resolve lookup");
//   auto bbContainerID =
//       fileSEXP[findParentClosureScope(pool, parentScope, iridiumBuildContext)];
//   IRI_STRUCTURAL::BBContainerSupport bbContainer(bbContainerID, pool);
//   IRI_STRUCTURAL::BindingsSupport next(bbContainer.getArg_Bindings(), pool);
//   IRI_GEN::ListSEXP remoteBindingsList(bindingsSEXP.getArg_RemoteBindings(),
//                                        pool);

//   auto updatedRemoteBindings = pool.get_args(remoteBindingsList.id);

//   IRI_GEN::IRID newlyCreatedRemoteBindingSEXP =
//       IRI_GEN::RemoteEnvBindingSEXP::create(
//           pool,
//           resolveScopedLookup(pool, fileSEXP, iridiumBuildContext, name,
//                               startScope, next),
//           false, updatedRemoteBindings.size());

//   updatedRemoteBindings.push_back(newlyCreatedRemoteBindingSEXP);
//   pool.set_args(remoteBindingsList.id, updatedRemoteBindings);
//   return newlyCreatedRemoteBindingSEXP;
// }

} // namespace IRI_HELPERS
