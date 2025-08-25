#include "Iridium/Passes/11_decorateReturnTargets.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"


typedef IridiumBuildContext::LoopConfig LoopConfig;
typedef IridiumBuildContext::TryContext TryContext;

typedef std::vector<std::variant<LoopConfig, TryContext>> IntermediateContextList;


IRIBUILDCONTEXT findReturnTarget(
  double localScope,
  std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext,
  IntermediateContextList & intermediateContexts
) {
  if (localScope == -1) throw std::runtime_error("Failed to find return target!!!");
  assert(iridiumBuildContext.find(localScope) != iridiumBuildContext.end());
  auto & buildContext = iridiumBuildContext[localScope];

  if (buildContext->tryContext)
  {
    intermediateContexts.push_back(buildContext->tryContext.value());
  }

  if (buildContext->loopConfig)
  {
    intermediateContexts.push_back(buildContext->loopConfig.value());
  }

  auto & startBB = buildContext->BB[0];

  if (startBB->hasTopLevel())
  {
    if (intermediateContexts.size() > 0) throw std::runtime_error("Top level return not expected to be wrapped inside intermediate contexts");
    if (buildContext->isModule)
    {
      throw std::runtime_error("Expected async returns in module top level code...");
    }
  }

  if (startBB->hasClosureBoundary()) return buildContext;

  return findReturnTarget(buildContext->parent, iridiumBuildContext, intermediateContexts);
}

void decorateReturnTargets(IRISEXP fileSEXP, IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP))
    return;
  
  if (auto bbSEXP  = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    std::unordered_map<IRISEXP, std::vector<std::variant<IridiumBuildContext::LoopConfig, IridiumBuildContext::TryContext>>> decoratorMap;

    for (int i = 0; i < bbSEXP->args.size(); i++)
    {
      if (auto rTarget  = std::dynamic_pointer_cast<ReturnSEXP>(bbSEXP->args.at(i)))
      {
        if (bbSEXP->hasTopLevel()) continue;
        if (rTarget->hasModuleEarlyReturn()) continue;

        std::vector<std::variant<LoopConfig, TryContext>> intermediateContextHolder;
        findReturnTarget(bbSEXP->getScopeIDX(), iridiumBuildContext, intermediateContextHolder);
        decoratorMap[bbSEXP->args.at(i)] = intermediateContextHolder;
      }
    }

    // Decorate emitted targets by handling requirements of the enclosing contexts
    for (auto & e : decoratorMap)
    {
      auto & element = e.first;
      auto & intermediateContexts = e.second;

      for (auto & intermediateContext : intermediateContexts)
      {
        if (auto loopConfig = std::get_if<LoopConfig>(&intermediateContext)) {
          if (loopConfig->kind == IridiumBuildContext::LoopConfig::Kind::ForOf)
          {
            auto stackReject = std::make_shared<StackRejectSEXP>(0);
            auto forOfIteratorClose = std::make_shared<JSForOfIteratorCloseSEXP>();
            stackReject->args.push_back(forOfIteratorClose);
            insertBefore(bbSEXP->args, element, stackReject);
          }
        } else if (auto tryContext = std::get_if<TryContext>(&intermediateContext)) {
          insertBefore(bbSEXP->args, element, std::make_shared<PopCatchContextSEXP>());
          if (tryContext->finalizerIDX > -1)
          {
            insertBefore(bbSEXP->args, element, std::make_shared<InvokeFinalizerSEXP>(tryContext->finalizerIDX));
          }
        }
      }
    }
  }
  for (auto & e : currSEXP->args) decorateReturnTargets(fileSEXP, e, iridiumBuildContext);
}