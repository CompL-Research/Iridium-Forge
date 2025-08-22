#include "Iridium/Passes/4_6_addClosureArgsBindings.h"
#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"

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
      auto argumentsSpecialObjectBinding = makeEnvBindingSEXP(-1, containerBC->scopeIdx, bindingName, flag, containerBC->scopeIdx, containerBC->parent);

      addToListSEXP(bindingsSEXP->getLocalBindings(), argumentsSpecialObjectBinding);

      auto stmt = makeJSImplicitBindingDeclarationSEXP(makeResolveEnvBindingSEXP(bindingName), makeListSEXP(), bindingName, flag, containerBC->argumentsKind == 1 ? 1 : 0);

      auto & startBB = containerBC->BB[0];
      prepend(startBB->args, stmt);
    }

    // Adding function arguments, if required
    for (int k = 0; k < containerBC->args.size(); k++) {
      EnvBindingSEXPKindFlag flag = EnvBindingSEXPKindFlag::JSARG;
      if (k + 1 == containerBC->args.size() && containerBC->hasRestArgs) {
        flag = EnvBindingSEXPKindFlag::JSRESTARG;
      }
      auto res = makeEnvBindingSEXP(k, containerBC->scopeIdx, containerBC->args[k], flag, containerBC->scopeIdx, containerBC->parent);

      addToListSEXP(bindingsSEXP->getLocalBindings(), res);
    }
  }
}