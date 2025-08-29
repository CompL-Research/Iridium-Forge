#include "Iridium/CorePasses/9_resolveLambdaTargets.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"



void resolveLambdaTargets(IRISEXP fileSEXP, IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, double currBBscope)
{
  
  // When patching bindings in the code region, remember to ignore the stack frame. Otherwise we will end up in an infinite loop...
  // Also, when wrapping something inside a new SEXP, prevent the recursive case...
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP) || std::dynamic_pointer_cast<PoolBindingSEXP>(currSEXP))
    return;
 
  for (int i = 0; i < currSEXP->args.size(); i++)
  {
    if (auto lSexp = std::dynamic_pointer_cast<LambdaSEXP>(currSEXP->args.at(i)))
    {
      auto targetScopeIDX = findParentClosureScope(currBBscope, iridiumBuildContext);
      if (targetScopeIDX < 0) throw std::runtime_error("A binding must resolve in a valid scope, none found");
      auto bbContainer = getBBContainerSEXPByScopeId(fileSEXP, targetScopeIDX);
      auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(bbContainer->getBindings());
      if (!bindingsSEXP) throw std::runtime_error("Couldnt find the expected BindingsSEXP");
      auto poolBinding = std::make_shared<PoolBindingSEXP>(lSexp, lSexp->getStartBBIDX(), bindingsSEXP->getLambdas()->args.size());
      addToListSEXP(bindingsSEXP->getLambdas(), poolBinding);
      currSEXP->args.at(i) = poolBinding;
    }
  }

  if (auto bb = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    for (auto & e : bb->args) resolveLambdaTargets(fileSEXP, e, iridiumBuildContext, bb->getScopeIDX());
  }
  else
  {
    for (auto & e : currSEXP->args) resolveLambdaTargets(fileSEXP, e, iridiumBuildContext, currBBscope);
  }
}