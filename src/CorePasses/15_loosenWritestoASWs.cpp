#include "Iridium/CorePasses/15_loosenWritestoASWs.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void loosenWritestoASWs(IRISEXP currSEXP)
{
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP) || std::dynamic_pointer_cast<PoolBindingSEXP>(currSEXP))
    return;

  if (auto envUpdate = std::dynamic_pointer_cast<EnvWriteSEXP>(currSEXP))
  {
    auto left = envUpdate->getLValTarget();
    if (auto lBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(left))
    {
      if (lBinding->hasASW()) envUpdate->setSAFE(true);
    }
  }

  if (auto envUpdate = std::dynamic_pointer_cast<JSExplicitBindingDeclarationSEXP>(currSEXP))
  {
    auto left = envUpdate->getLValTarget();
    if (auto lBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(left))
    {
      if (lBinding->hasASW()) envUpdate->setSAFE(true);
    }
  }

  for (auto & e : currSEXP->args) loosenWritestoASWs(e);
}