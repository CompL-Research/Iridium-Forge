#include "Iridium/CorePasses/18_reduceDeleteNonGlobalBindingsToTrue.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"


static void doPatch(IRISEXP fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, IRISEXP currNode, double currScope) {
  for (int i = 0; i < currNode->args.size(); i++) {
    auto currSEXP = currNode->args.at(i);
    if (auto jsUnop = std::dynamic_pointer_cast<JSUnopSEXP>(currSEXP)) {
      if (auto unopToDel = std::dynamic_pointer_cast<UNOPDelVarSEXP>(jsUnop->getVal())) {
        // delete VAR_NAME iff VAR_NAME is not a global binding and replace the entire construct with "false"
        auto targetScopeIDX = findParentClosureScope(currScope, iridiumBuildContext);
        if (targetScopeIDX < 0)
          throw std::runtime_error("[18] A binding must resolve in a valid scope, none found for: " + std::to_string(targetScopeIDX));
        auto bbContainer = getBBContainerSEXPByScopeId(fileSEXP, targetScopeIDX);
        std::shared_ptr<BindingsSEXP> bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(bbContainer->getBindings());
        if (!bindingsSEXP)
          throw std::runtime_error("Bindings not found, it is needed to resolve bindings");
        bool isGlobal = isGlobalBinding(fileSEXP, iridiumBuildContext, unopToDel->getNAME(), currScope, bindingsSEXP);
        if (!isGlobal) {
          currNode->args.at(i) = std::make_shared<BooleanSEXP>(false);
        }
      }
    }
  }
  for (auto e : currNode->args) doPatch(fileSEXP, iridiumBuildContext, e, currScope);
}

void reduceDeleteNonGlobalBindingsToTrue(IRISEXP file, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
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
      for (int i = 0; i < bb->args.size(); i++) {
        doPatch(fileSEXP, iridiumBuildContext, bb->args.at(i), bbSEXP->getScopeIDX());
      }
    }
  }
}