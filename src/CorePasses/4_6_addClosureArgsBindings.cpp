#include "Iridium/CorePasses/4_6_addClosureArgsBindings.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void addClosureArgsBindings(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  // Iterate over BBContainerSEXP
  for (auto &bbc : fileSexp->args)
  {
    auto container = std::dynamic_pointer_cast<BBContainerSEXP>(bbc);
    if (!container)
      continue;
    assert(iridiumBuildContext.find(container->getScopeIDX()) != iridiumBuildContext.end());
    auto containerBC = iridiumBuildContext[container->getScopeIDX()];

    auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(container->getBindings());
    assert(bindingsSEXP && "Expected bindingsSEXP");
    


    // Adding function arguments, if required
    for (int k = 0; k < containerBC->args.size(); k++) {
      EnvBindingSEXPKindFlag flag = EnvBindingSEXPKindFlag::JSARG;
      if (k + 1 == containerBC->args.size() && containerBC->hasRestArgs) {
        flag = EnvBindingSEXPKindFlag::JSRESTARG;
      }

      // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
      auto res = std::make_shared<EnvBindingSEXP>(containerBC->args[k], false, flag == EnvBindingSEXPKindFlag::JSARG, flag == EnvBindingSEXPKindFlag::JSRESTARG, false, false, false, containerBC->scopeIdx, k, containerBC->scopeIdx, containerBC->parent, -1);

      addToListSEXP(bindingsSEXP->getLocalBindings(), res);
    }
  }
}