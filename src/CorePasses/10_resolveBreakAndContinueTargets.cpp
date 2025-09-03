#include "Iridium/CorePasses/10_resolveBreakAndContinueTargets.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"


LoopConfig findLoopControlTarget(
  double localScope,
  std::variant<std::shared_ptr<ResolveBreakTargetSEXP>, std::shared_ptr<ResolveContinueTargetSEXP>> node,
  std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext,
  std::vector<std::variant<LoopConfig, TryContext>> & intermediateContexts
) {
  if (localScope == -1) throw std::runtime_error("Failed to find loop control target!!!");
  assert(iridiumBuildContext.find(localScope) != iridiumBuildContext.end());
  auto & buildContext = iridiumBuildContext[localScope];

  if (buildContext->tryContext)
  {
    intermediateContexts.push_back(buildContext->tryContext.value());
  }

  // If not loop context, recurse
  if (!buildContext->loopConfig) return findLoopControlTarget(buildContext->parent, node, iridiumBuildContext, intermediateContexts);

  auto & loopConfig = buildContext->loopConfig;

  bool hasLabel = false;
  std::string label;

  if (auto breakTarget = std::get_if<std::shared_ptr<ResolveBreakTargetSEXP>>(&node)) {
    hasLabel = breakTarget->get()->hasLabel();
    if (hasLabel) label = breakTarget->get()->getLabel();
  } else if (auto continueTarget = std::get_if<std::shared_ptr<ResolveContinueTargetSEXP>>(&node)) {
    hasLabel = continueTarget->get()->hasLabel();
    if (hasLabel) label = continueTarget->get()->getLabel();
  }

  // Intermediate loop context, but not the one we are trying to flow to
  if (hasLabel) {
    if ((!loopConfig->label) || (label != loopConfig->label.value())) {
      intermediateContexts.push_back(loopConfig.value());
      return findLoopControlTarget(buildContext->parent, node, iridiumBuildContext, intermediateContexts);
    }
  }
  
  // If we are looking for a continue target, but the resolved target does not have a continue target, keep looking...
  if (std::holds_alternative<std::shared_ptr<ResolveContinueTargetSEXP>>(node) && loopConfig->continueTarget == -1) {
    intermediateContexts.push_back(loopConfig.value());
    return findLoopControlTarget(buildContext->parent, node, iridiumBuildContext, intermediateContexts);
  }

  return loopConfig.value();
}

void resolveBreakAndContinueTargets(IRISEXP fileSEXP, IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP))
    return;
  
  if (auto bbSEXP  = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    std::unordered_map<IRISEXP, std::vector<std::variant<LoopConfig, TryContext>>> decoratorMap;
    std::unordered_map<IRISEXP, LoopConfig> isBreakTarget;

    for (int i = 0; i < bbSEXP->args.size(); i++)
    {
      if (auto bTarget  = std::dynamic_pointer_cast<ResolveBreakTargetSEXP>(bbSEXP->args.at(i)))
      {
        std::vector<std::variant<LoopConfig, TryContext>> intermediateContextHolder;
        auto target = findLoopControlTarget(bbSEXP->getScopeIDX(), bTarget, iridiumBuildContext, intermediateContextHolder);
        auto gotoSEXP = std::make_shared<GotoSEXP>(target.breakTarget);
        bbSEXP->args.at(i) = gotoSEXP;
        decoratorMap[gotoSEXP] = intermediateContextHolder;
        isBreakTarget[gotoSEXP] = target;
      }

      if (auto cTarget  = std::dynamic_pointer_cast<ResolveContinueTargetSEXP>(bbSEXP->args.at(i)))
      {
        std::vector<std::variant<LoopConfig, TryContext>> intermediateContextHolder;
        auto target = findLoopControlTarget(bbSEXP->getScopeIDX(), cTarget, iridiumBuildContext, intermediateContextHolder);
        auto gotoSEXP = std::make_shared<GotoSEXP>(target.continueTarget);
        bbSEXP->args.at(i) = gotoSEXP;
        decoratorMap[gotoSEXP] = intermediateContextHolder;
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
          if (loopConfig->kind == LoopConfig::Kind::ForOf)
          {
            auto stackReject = std::make_shared<StackRejectSEXP>(0);
            auto forOfIteratorClose = std::make_shared<JSForOfIteratorCloseSEXP>();
            stackReject->args.push_back(forOfIteratorClose);
            insertBefore(bbSEXP->args, element, stackReject);
          }
        } else if (auto tryContext = std::get_if<TryContext>(&intermediateContext)) {
          // Pop only if the reaching scope context is through try or catch context, do nothing for finalizer contexts...
          if (tryContext->finalizerIDX == -1)
          {
            insertBefore(bbSEXP->args, element, std::make_shared<PopCatchContextSEXP>());
          }
          else
          {
            if (
                hasScopePath(bbSEXP->getScopeIDX(), getBBScopeIDX(tryContext->tryIDX, iridiumBuildContext), iridiumBuildContext) || (tryContext->udCatchIDX > -1 && hasScopePath(bbSEXP->getScopeIDX(), getBBScopeIDX(tryContext->udCatchIDX, iridiumBuildContext), iridiumBuildContext)))
            {
              insertBefore(bbSEXP->args, element, std::make_shared<PopCatchContextSEXP>());
              insertBefore(bbSEXP->args, element, std::make_shared<InvokeFinalizerSEXP>(tryContext->finalizerIDX));
            }
            else
            {
              assert(hasScopePath(bbSEXP->getScopeIDX(), getBBScopeIDX(tryContext->finalizerIDX, iridiumBuildContext), iridiumBuildContext));
            }
          }
        }
      }

      if (isBreakTarget.find(element) != isBreakTarget.end())
      {
        auto & finalLoopConfig = isBreakTarget[element];
        if (finalLoopConfig.kind == LoopConfig::Kind::ForOf)
        {
          auto stackReject = std::make_shared<StackRejectSEXP>(0);
          auto forOfIteratorClose = std::make_shared<JSForOfIteratorCloseSEXP>();
          stackReject->args.push_back(forOfIteratorClose);
          insertBefore(bbSEXP->args, element, stackReject);
        }
      }
    }
  }
  for (auto & e : currSEXP->args) resolveBreakAndContinueTargets(fileSEXP, e, iridiumBuildContext);
}