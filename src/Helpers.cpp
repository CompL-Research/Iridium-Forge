#include "Helpers.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/StringPool.h"
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
  if (pool[containedBinding].tag == IRI_GEN::EnvBinding) return containedBinding;
  else return resolveRemoteBinding(pool, rbinID);
}

double getTopLevelScope(IRI_STORAGE::IridiumPool &pool,
                        IRI_STORAGE::IRID fileSEXP) {
  IRI_GEN::IRID topLevelContainer = getTopLevelContainer(pool, fileSEXP);
  IRI_GEN::BBContainerSEXP c(topLevelContainer, pool);

  return c.getScopeIDX();
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

IRI_STORAGE::IRID createNoASWResolveEnvBindingSEXP(IRI_STORAGE::IridiumPool & pool, StringID s) {
  return IRI_GEN::ResolveEnvBindingSEXP::create(pool, s, false);
}

IRI_STORAGE::IRID createUnsafeEnvReadSEXP(IRI_STORAGE::IridiumPool & pool, StringID s) {
  return IRI_GEN::EnvReadSEXP::create(pool, createNoASWResolveEnvBindingSEXP(pool, s), false);
}

} // namespace IRI_HELPERS
