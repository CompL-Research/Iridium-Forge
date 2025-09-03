#include "Iridium/CorePasses/11_decorateReturnTargets.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void decorateReturnTargets(IRISEXP fileSEXP, IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP))
    return;

  if (auto bbSEXP = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    std::unordered_map<IRISEXP, std::vector<std::variant<LoopConfig, TryContext>>> decoratorMap;

    for (int i = 0; i < bbSEXP->args.size(); i++)
    {
      if (auto rTarget = std::dynamic_pointer_cast<ReturnSEXP>(bbSEXP->args.at(i)))
      {
        if (bbSEXP->hasTopLevel())
          continue;
        if (rTarget->hasModuleEarlyReturn())
          continue;

        std::vector<std::variant<LoopConfig, TryContext>> intermediateContextHolder;
        findReturnTarget(bbSEXP->getScopeIDX(), iridiumBuildContext, intermediateContextHolder);
        decoratorMap[bbSEXP->args.at(i)] = intermediateContextHolder;
      }
    }

    // Decorate emitted targets by handling requirements of the enclosing contexts
    for (auto &e : decoratorMap)
    {
      auto &element = e.first;
      auto &intermediateContexts = e.second;

      for (auto &intermediateContext : intermediateContexts)
      {
        if (auto loopConfig = std::get_if<LoopConfig>(&intermediateContext))
        {
          if (loopConfig->kind == LoopConfig::Kind::ForOf)
          {
            auto stackReject = std::make_shared<StackRejectSEXP>(0);
            auto forOfIteratorClose = std::make_shared<JSForOfIteratorCloseSEXP>();
            stackReject->args.push_back(forOfIteratorClose);
            insertBefore(bbSEXP->args, element, stackReject);
          }
        }
        else if (auto tryContext = std::get_if<TryContext>(&intermediateContext))
        {
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
    }
  }
  for (auto &e : currSEXP->args)
    decorateReturnTargets(fileSEXP, e, iridiumBuildContext);
}