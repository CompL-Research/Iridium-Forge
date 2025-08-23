#include "Iridium/Passes/4_1_groupIntoClosureGroups.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void groupIntoClosureGroups(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  std::unordered_map<double, std::shared_ptr<BBContainerSEXP>> bbGroups;

  for (auto &bb : fileSexp->args)
  {
    auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);
    assert(currBB && "Expected a BBSEXP");

    auto targetScopeIDX = findParentClosureScope(currBB->getScopeIDX(), iridiumBuildContext);

    if (bbGroups.find(targetScopeIDX) == bbGroups.end())
    {
      auto parent = getLexicalScope(targetScopeIDX, iridiumBuildContext);


      auto localBindings = std::make_shared<ListSEXP>("EnvBinding");
      auto remoteBindings = std::make_shared<ListSEXP>("RemoteEnvBinding");
      auto poolBindings = std::make_shared<ListSEXP>("PoolBinding");

      auto bindingsSEXP = std::make_shared<BindingsSEXP>(localBindings, remoteBindings, poolBindings, parent);
      auto BBListSEXP = std::make_shared<ListSEXP>("BB");

      auto &currContext = iridiumBuildContext[targetScopeIDX];

      bool isTopLevel = parent == -1;

      auto bbContainer = std::make_shared<BBContainerSEXP>(bindingsSEXP, BBListSEXP, false, currContext->isAsync, currContext->isStrict, currContext->isGenerator, false, false, false, false, false, false, isTopLevel, currContext->ecmaArgs, currContext->BB[0]->getIDX(), targetScopeIDX, -1);

      setClosureFlags(currContext->kind, bbContainer);
      bbGroups[targetScopeIDX] = bbContainer;
    }

    addToListSEXP(bbGroups[targetScopeIDX]->getBB(), currBB);
  }

  // Replace all BB's in the FileSEXP with the generated Containers.
  fileSexp->args.clear();
  fileSexp->args.reserve(bbGroups.size());
  for (auto &e : bbGroups)
  {
    fileSexp->args.push_back(std::move(e.second));
  }
}