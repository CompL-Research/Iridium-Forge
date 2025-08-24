#include "Iridium/Passes/7_reduceResolvePrivateEnvBindingSEXP.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void reduceResolvePrivateEnvBindingSEXP(IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, double currBBScope)
{
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP))
    return;

  for (int i = 0; i < currSEXP->args.size(); i++)
  {
    auto & s = currSEXP->args[i];
    if (auto pvt = std::dynamic_pointer_cast<ResolvePrivateEnvBindingSEXP>(s))
    {
      auto pvtName = pvt->getNAME();
      auto targetScopeIDX = findParentClosureScope(currBBScope, iridiumBuildContext);
      while (true)
      {
        assert(iridiumBuildContext.find(targetScopeIDX) != iridiumBuildContext.end());
        auto buildContext = iridiumBuildContext[targetScopeIDX];
        if (buildContext->privateMapping)
        {
          auto & privateMapping = buildContext->privateMapping.value();
          if (privateMapping.find(pvtName) != privateMapping.end())
          {
            auto & pvtInfo = privateMapping[pvtName];

            // IRISEXP Obj, bool SYMBOL, bool METHOD, bool FULLY_RESOLVE
            currSEXP->args.at(i) = std::make_shared<PVTEnvReadSEXP>(
              std::make_shared<ResolveEnvBindingSEXP>(pvtInfo.first, false),
              pvtInfo.second == "SYMBOL", 
              pvtInfo.second == "METHOD", 
              pvt->hasFULLY_RESOLVE());
            break;
          }
        }

        targetScopeIDX = findParentClosureScope(buildContext->parent, iridiumBuildContext);
        if (targetScopeIDX < 0) throw std::runtime_error("Failed to resolve private binding!!!");
      }
    }
  }

  if (auto bb = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    for (auto & e : currSEXP->args) reduceResolvePrivateEnvBindingSEXP(e, iridiumBuildContext, bb->getScopeIDX());
  }
  else
  {
    for (auto & e : currSEXP->args) reduceResolvePrivateEnvBindingSEXP(e, iridiumBuildContext, currBBScope);
  }
}