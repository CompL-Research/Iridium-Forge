#include "Iridium/CorePasses/16_markDirectEvals.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

static std::shared_ptr<BindingsSEXP> lastBindingsObject = NULL;

bool isGenericCall(std::shared_ptr<CallSiteSEXP> o) {
  if (o->hasImport()) return false;
  else if (o->hasSuper()) return false;
  else if (o->hasV8Intrinsic()) return false;
  else if (o->hasCCall()) return false;
  else if (o->hasConstructorCall()) return false;
  else if (o->hasPrivateCall()) return false;
  else if (o->hasJSDirectEval()) return false;
  else return true; 
}

std::vector<std::shared_ptr<EnvBindingSEXP>> filterLocalBindingsByScope(std::vector<IRISEXP> bindingsObjLocalBindings, double currLookup)
{
  std::vector<std::shared_ptr<EnvBindingSEXP>> result;

  for (auto & e : bindingsObjLocalBindings)
  {
    if (auto b = std::dynamic_pointer_cast<EnvBindingSEXP>(e))
    {
      if (b->getScope() == currLookup) result.push_back(b);
    } else throw std::runtime_error("This function must only be called on a vector containing EnvBindingSEXP objects!!");
  }


  std::sort(result.begin(), result.end(),
  [](std::shared_ptr<EnvBindingSEXP> &a, std::shared_ptr<EnvBindingSEXP> &b) {
    return a->getREFIDX() < b->getREFIDX();
  });

  return result;
}


void markDirectEvals(IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, double currBBScope)
{
  if (auto bObj = std::dynamic_pointer_cast<BindingsSEXP>(currSEXP))
  {
    lastBindingsObject = bObj;
  }

  if (auto callSiteSEXP = std::dynamic_pointer_cast<CallSiteSEXP>(currSEXP))
  {
    if (isGenericCall(callSiteSEXP))
    {
      auto callee = callSiteSEXP->args[0];

      if (auto globalBindingCallee = std::dynamic_pointer_cast<GlobalBindingSEXP>(callee))
      {
        if (globalBindingCallee->getNAME() == "eval")
        {
          auto currLookup = currBBScope;
          if (!lastBindingsObject) throw std::runtime_error("bindingsObj missing...");
          if (currLookup < 0) throw std::runtime_error("how is this less than zero?");
          
          auto bindingsObjLocalBindings = lastBindingsObject->getLocalBindings()->args;

          do {
            auto bs = filterLocalBindingsByScope(bindingsObjLocalBindings, currLookup);

            if (bs.size() > 0)
            {
              callSiteSEXP->setJSDirectEval(bs.back()->getREFIDX());
              break;
            }
            else
            {
              currLookup = getLexicalScope(currLookup, iridiumBuildContext);
              if (currLookup == -1) {
                callSiteSEXP->setJSDirectEval(0);
                break;
              }
            }
          } while (true);
        }
      }
    }
  }
  if (auto bb = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    for (auto & e : bb->args) markDirectEvals(e, iridiumBuildContext, bb->getScopeIDX());
  }
  else
  {
    for (auto & e : currSEXP->args) markDirectEvals(e, iridiumBuildContext, currBBScope);
  }

}