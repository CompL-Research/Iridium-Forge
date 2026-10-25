#include "Helpers.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include "Storage/StringPool.h"
#include "Support/BBContainerSupport.hpp"
#include <functional>
#include <stdexcept>

namespace IRI_HELPERS {

auto findVARHoistingScope(
    IRI_STORAGE::IRIContext &ctx, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) -> double {
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " +
                             std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  IRI_GEN::BBSEXP startBB(buildContext->BB[0], ctx);
  if (startBB.hasClosureBoundary() || startBB.hasTopLevel() ||
      startBB.hasVARBoundary())
    return startingScope;
  return findVARHoistingScope(ctx, buildContext->parent, iridiumBuildContext);
}

auto findParentClosureScope(
    IRI_STORAGE::IRIContext &ctx, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) -> double {
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " +
                             std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  IRI_GEN::BBSEXP startBB(buildContext->BB.at(0), ctx);

  if (startBB.hasClosureBoundary() || startBB.hasTopLevel())
    return startingScope;
  return findParentClosureScope(ctx, buildContext->parent,
                                iridiumBuildContext);
}

auto getLexicalScope(
    IRI_STORAGE::IRIContext &ctx, double startingScope,
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

IRI_STORAGE::IRID getTopLevelContainer(IRI_STORAGE::IRIContext &ctx,
                                       IRI_STORAGE::IRID fileSEXP) {
  for (auto &bbcIDX : ctx.storage.nodes.get_args(fileSEXP)) {
    if (IRI_NODE(ctx, bbcIDX).tag != IRI_GEN::BBContainer)
      continue;

    auto container = IRI_GEN::BBContainerSEXP(bbcIDX, ctx);
    if (container.hasTopLevel()) {
      return bbcIDX;
    }
  }
  throw std::runtime_error("[Forge] Failed to find top level container SEXP");
}
IRI_STORAGE::IRID resolveRemoteBinding(IRI_STORAGE::IRIContext &ctx,
                                       IRI_STORAGE::IRID rbinID) {
  IRI_GEN::RemoteEnvBindingSEXP rbin(rbinID, ctx);
  IRI_STORAGE::IRID containedBinding = rbin.getArg_ParentReference();
  if (IRI_NODE(ctx, containedBinding).tag == IRI_GEN::EnvBinding) {
    return containedBinding;
  } else if (IRI_NODE(ctx, containedBinding).tag == IRI_GEN::RemoteEnvBinding) {
    return resolveRemoteBinding(ctx, containedBinding);
  } else {
    throw std::runtime_error("[Forge] Failed to resolve RemoteEnvBinding");
  }
}

double getTopLevelScope(IRI_STORAGE::IRIContext &ctx,
                        IRI_STORAGE::IRID fileSEXP) {
  IRI_GEN::IRID topLevelContainer = getTopLevelContainer(ctx, fileSEXP);
  IRI_GEN::BBContainerSEXP c(topLevelContainer, ctx);

  return c.getScopeIDX();
}

void countNodeOccurenceWithPredicate(
    IRI_STORAGE::IRID id, IRI_STORAGE::IRIContext *ctx,
    std::function<bool(IRI_STORAGE::IRID)> pred, size_t &count) {

  if (pred(id))
    count++;
  for (auto &i : ctx->storage.nodes.get_args_view(id))
    countNodeOccurenceWithPredicate(i, ctx, pred, count);
}

bool hasNodeWithPredicate(IRI_GEN::IRID id, IRI_GEN::IRIContext *ctx,
                          std::function<bool(IRI_STORAGE::IRID)> pred) {

  if (pred(id))
    return true;
  for (auto &i : ctx->storage.nodes.get_args_view(id))
    if (hasNodeWithPredicate(i, ctx, pred))
      return true;

  return false;
}

IRI_STORAGE::IRID
createNoASWResolveEnvBindingSEXP(IRI_STORAGE::IRIContext &ctx, StringID s) {
  return IRI_GEN::ResolveEnvBindingSEXP::create(ctx, s, false);
}

IRI_STORAGE::IRID createUnsafeEnvReadSEXP(IRI_STORAGE::IRIContext &ctx,
                                          StringID s) {
  return IRI_GEN::EnvReadSEXP::create(
      ctx, createNoASWResolveEnvBindingSEXP(ctx, s), false, false);
}

} // namespace IRI_HELPERS
