#include "Iridium/CorePasses/12_promoteAsyncReturns.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"


void promoteAsyncReturns(IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP))
    return;
  
  if (auto bbSEXP  = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    for (int i = 0; i < bbSEXP->args.size(); i++)
    {
      if (auto rTarget  = std::dynamic_pointer_cast<ReturnSEXP>(bbSEXP->args.at(i)))
      {
        if (bbSEXP->hasTopLevel()) continue;
        if (rTarget->hasModuleEarlyReturn()) continue;

        std::vector<std::variant<LoopConfig, TryContext>> intermediateContextHolder;
        auto target = findReturnTarget(bbSEXP->getScopeIDX(), iridiumBuildContext, intermediateContextHolder);

        if (target->isAsync || target->isGenerator)
        {
          bbSEXP->args.at(i) = std::make_shared<ReturnAsyncSEXP>(rTarget->getObj());
        }
      }
    }
  }
  for (auto & e : currSEXP->args) promoteAsyncReturns(e, iridiumBuildContext);
}