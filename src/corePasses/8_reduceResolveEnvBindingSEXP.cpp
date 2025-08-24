#include "Iridium/Passes/8_reduceResolveEnvBindingSEXP.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

static void reduceResolveEnvBindingSEXPHelper(IRISEXP fileSEXP, IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, double currBBScope)
{
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP))
    return;

  for (int i = 0; i < currSEXP->args.size(); i++)
  {
    auto & s = currSEXP->args[i];
    if (auto uBinding = std::dynamic_pointer_cast<ResolveEnvBindingSEXP>(s))
    {

      auto isASW = uBinding->hasASW();
      auto targetScopeIDX = findParentClosureScope(currBBScope, iridiumBuildContext);
      if (targetScopeIDX < 0) throw std::runtime_error("A binding must resolve in a valid scope, none found");

      auto bbContainer = getBBContainerSEXPByScopeId(fileSEXP, targetScopeIDX);

      std::shared_ptr<BindingsSEXP> bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(bbContainer->getBindings());
      if (!bindingsSEXP) throw std::runtime_error("Bindings not found, it is needed to resolve bindings");

      auto globalBinding = isGlobalBinding(fileSEXP, iridiumBuildContext, uBinding->getNAME(), currBBScope, bindingsSEXP);
      if (globalBinding) {
        if (isASW) throw std::runtime_error("Tried to mark a global binding as an ASW binding");
          currSEXP->args.at(i) = std::make_shared<GlobalBindingSEXP>(uBinding->getNAME());
      }
      else
      {
        auto resolvedBinding = resolveScopedLookup(fileSEXP, iridiumBuildContext, uBinding->getNAME(), currBBScope, bindingsSEXP);

        if (auto rrr = std::dynamic_pointer_cast<EnvBindingSEXP>(resolvedBinding)) {
          if (isASW) rrr->setASW();
        }

        currSEXP->args.at(i) = resolvedBinding;
      }
    }
  }

  if (auto bb = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    for (auto & e : currSEXP->args) reduceResolveEnvBindingSEXPHelper(fileSEXP, e, iridiumBuildContext, bb->getScopeIDX());
  }
  else
  {
    for (auto & e : currSEXP->args) reduceResolveEnvBindingSEXPHelper(fileSEXP, e, iridiumBuildContext, currBBScope);
  }
}

void reduceResolveEnvBindingSEXP(IRISEXP fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{

  reduceResolveEnvBindingSEXPHelper(fileSEXP, fileSEXP, iridiumBuildContext, 0);
}