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
    

    // 0 -> No Arguments Object
    // 1 -> Mapped Arguments
    // 2 -> Unmapped Arguments
    if (containerBC->argumentsKind > 0) {
      container->setARGUMENTS();
      std::string bindingName = "arguments";
      EnvBindingSEXPKindFlag flag = EnvBindingSEXPKindFlag::JSVAR;
      // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
      auto argumentsSpecialObjectBinding = std::make_shared<EnvBindingSEXP>(bindingName, false, false, false, false, false, true, containerBC->scopeIdx, -1, containerBC->scopeIdx, containerBC->parent, -1);

      addToListSEXP(bindingsSEXP->getLocalBindings(), argumentsSpecialObjectBinding);

      // IRISEXP Store, IRISEXP Args, std::string NAME, bool JSLET, bool JSCONST, bool JSVAR, bool SLOPPY, bool SKIPINIT, bool SAFE, bool THISINIT, double OPID
      auto stmt = std::make_shared<JSImplicitBindingDeclarationSEXP>(
        std::make_shared<ResolveEnvBindingSEXP>(bindingName, false),
        std::make_shared<ListSEXP>(""),
        bindingName,
        false,
        false,
        true,
        false,
        true,
        true,
        false,
        containerBC->argumentsKind == 1 ? 1 : 0
      );
      
      auto & startBB = containerBC->BB[0];
      prepend(startBB->args, stmt);
    }

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