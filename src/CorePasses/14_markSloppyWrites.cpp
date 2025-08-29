#include "Iridium/CorePasses/14_markSloppyWrites.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void markSloppyWrites(IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, double currBBScope)
{
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP) || std::dynamic_pointer_cast<PoolBindingSEXP>(currSEXP))
    return;

  if (auto envUpdate = std::dynamic_pointer_cast<EnvWriteSEXP>(currSEXP))
  {
    assert(iridiumBuildContext.find(currBBScope) != iridiumBuildContext.end());
    auto & buildContext = iridiumBuildContext[currBBScope];
    if (!buildContext->isStrict)
    {
      envUpdate->setSLOPPY();
    }
  }

  if (auto envUpdate = std::dynamic_pointer_cast<JSExplicitBindingDeclarationSEXP>(currSEXP))
  {
    assert(iridiumBuildContext.find(currBBScope) != iridiumBuildContext.end());
    auto & buildContext = iridiumBuildContext[currBBScope];
    if (!buildContext->isStrict)
    {
      envUpdate->setSLOPPY();
    }
  }

  if (auto envUpdate = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(currSEXP))
  {
    assert(iridiumBuildContext.find(currBBScope) != iridiumBuildContext.end());
    auto & buildContext = iridiumBuildContext[currBBScope];
    if (!buildContext->isStrict)
    {
      envUpdate->setSLOPPY();
    }
  }

  if (auto bb = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    for (auto & e : bb->args) markSloppyWrites(e, iridiumBuildContext, bb->getScopeIDX());
  }
  else
  {
    for (auto & e : currSEXP->args) markSloppyWrites(e, iridiumBuildContext, currBBScope);
  }

  
}