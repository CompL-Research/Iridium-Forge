#include "Iridium/Passes/4_1_groupIntoClosureGroups.h"
#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"
#include "Iridium/IridiumConstructors.h"

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
      auto bbContainer = makeBBContainerSEXP(targetScopeIDX, parent);
      auto &currContext = iridiumBuildContext[targetScopeIDX];

      if (currContext->isAsync)
        bbContainer->setASYNC();
      if (currContext->isGenerator)
        bbContainer->setGENERATOR();
      if (currContext->isStrict)
        bbContainer->setSTRICT();
      if (parent == -1)
        bbContainer->setTopLevel();

      bbContainer->setECMAArgs(currContext->ecmaArgs);

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