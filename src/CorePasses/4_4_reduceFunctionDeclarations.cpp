#include "Iridium/CorePasses/4_4_reduceFunctionDeclarations.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include "Iridium/IridiumReductions.h"

void reduceFunctionDeclarations(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  bool isModule = false;
  double topLevelScopeIdx = 0;

  // Iterate over BBContainerSEXP
  for (auto &bbc : fileSexp->args)
  {
    auto container = std::dynamic_pointer_cast<BBContainerSEXP>(bbc);
    if (!container)
      continue;
    assert(iridiumBuildContext.find(container->getScopeIDX()) != iridiumBuildContext.end());
    auto containerBC = iridiumBuildContext[container->getScopeIDX()];
    bool isTopLevel = container->hasTopLevel();

    if (isTopLevel)
    {
      isModule = containerBC->isModule;
      topLevelScopeIdx = containerBC->scopeIdx;
    }
    auto bbs = std::dynamic_pointer_cast<ListSEXP>(container->getBB());
    assert(bbs && "Expected ListSEXP");
    for (auto &_bb : bbs->args)
    {
      auto bb = std::dynamic_pointer_cast<BBSEXP>(_bb);
      assert(bb && "Expected bb to be a BBSEXP");

      auto localScope = bb->getScopeIDX();
      auto parentClosureScope = findParentClosureScope(localScope, iridiumBuildContext);

      // Iterate over STMT
      for (int i = 0; i < bb->args.size(); i++)
      {
        auto stmt = bb->args.at(i);
        //
        // Function Declaration, reduce it to an Explicit Binding Declaration if possible.
        //
        if (auto funcDeclStmt = std::dynamic_pointer_cast<JSFuncDeclSEXP>(bb->args.at(i)))
        {
          if (!isModule && localScope == topLevelScopeIdx)
          {
            // NADA
          }
          else
          {
            bb->args.at(i) = reduceJSFunDecl(funcDeclStmt);
          }
        }
      }
    }
  }
}