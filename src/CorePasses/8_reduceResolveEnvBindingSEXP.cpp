#include "Iridium/CorePasses/8_reduceResolveEnvBindingSEXP.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include <functional>

static void reduceResolveEnvBindingSEXPHelper(IRISEXP fileSEXP, IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, double currBBScope)
{

  for (int i = 0; i < currSEXP->args.size(); i++)
  {
    auto &s = currSEXP->args.at(i);

    std::function<IRISEXP(bool, std::string, double)> resolveBinding = [&](bool isASW, std::string bindingName, double lookupStartScope)
    {
      auto targetScopeIDX = findParentClosureScope(lookupStartScope, iridiumBuildContext);
      if (targetScopeIDX < 0)
        throw std::runtime_error("A binding must resolve in a valid scope, none found");
      auto bbContainer = getBBContainerSEXPByScopeId(fileSEXP, targetScopeIDX);
      std::shared_ptr<BindingsSEXP> bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(bbContainer->getBindings());
      if (!bindingsSEXP)
        throw std::runtime_error("Bindings not found, it is needed to resolve bindings");
      auto globalBinding = isGlobalBinding(fileSEXP, iridiumBuildContext, bindingName, lookupStartScope, bindingsSEXP);
      if (globalBinding)
      {
        if (isASW)
          throw std::runtime_error("Tried to mark a global binding as an ASW binding: " + bindingName);
        return std::static_pointer_cast<IridiumSEXP>(std::make_shared<GlobalBindingSEXP>(bindingName));
      }
      else
      {
        auto resolvedBinding = resolveScopedLookup(fileSEXP, iridiumBuildContext, bindingName, lookupStartScope, bindingsSEXP);
        if (isASW)
          if (auto rrr = std::dynamic_pointer_cast<EnvBindingSEXP>(resolvedBinding))
            rrr->setASW();
        return resolvedBinding;
      }
    };

    if (auto siblingSpecialWrite = std::dynamic_pointer_cast<SiblingSpecialWriteSEXP>(s))
    {
      auto lval = std::dynamic_pointer_cast<ResolveEnvBindingSEXP>(siblingSpecialWrite->getLValTarget());
      assert(lval && "Expected LVAL of SiblingSpecialWrite to be a ResolveEnvBindingSEXP");
      // For LVAL, ensure its a ResolveEnvBindingSEXP and bypass lookup scope to sibling...

      // IRISEXP LValTarget, IRISEXP RVal, bool SLOPPY, bool THROWERR, bool SAFE, bool THISINIT
      auto updatedStmt = std::make_shared<EnvWriteSEXP>(
          // Writing to arguments is always safe
          resolveBinding(true, lval->getNAME(), siblingSpecialWrite->getScopeIDX()),
          siblingSpecialWrite->getRVal(),
          siblingSpecialWrite->hasSLOPPY(),
          siblingSpecialWrite->hasTHROWERR(),
          true, // Writing to arguments is always safe
          siblingSpecialWrite->getTHISINIT());
      currSEXP->args.at(i) = updatedStmt;

      // Process RVal Normally
      reduceResolveEnvBindingSEXPHelper(fileSEXP, updatedStmt->getRVal(), iridiumBuildContext, currBBScope);

      continue;
    }
    if (auto uBinding = std::dynamic_pointer_cast<ResolveEnvBindingSEXP>(s))
    {
      auto isASW = uBinding->hasASW();
      auto binding = uBinding->getNAME();
      currSEXP->args.at(i) = resolveBinding(isASW, binding, currBBScope);
      continue;
    }

    reduceResolveEnvBindingSEXPHelper(fileSEXP, s, iridiumBuildContext, currBBScope);
  }
}

void reduceResolveEnvBindingSEXP(IRISEXP file, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  auto fileSEXP = std::dynamic_pointer_cast<FileSEXP>(file);
  assert(fileSEXP && "Expected FileSEXP");
  for (auto bbCont : fileSEXP->args)
  {
    auto bbContainerSEXP = std::dynamic_pointer_cast<BBContainerSEXP>(bbCont);
    if (!bbContainerSEXP)
      continue;
    for (auto bb : bbContainerSEXP->getBB()->args)
    {
      auto bbSEXP = std::dynamic_pointer_cast<BBSEXP>(bb);
      assert(bbSEXP && "Expected BBSEXP");
      reduceResolveEnvBindingSEXPHelper(fileSEXP, bbSEXP, iridiumBuildContext, bbSEXP->getScopeIDX());
    }
  }
}