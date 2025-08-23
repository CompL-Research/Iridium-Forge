#include "Iridium/Passes/4_2_populateModuleBindings.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void populateModuleBindings(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  auto moduleRequests = std::make_shared<ListSEXP>("ModuleRequest");

  auto staticImports = std::make_shared<ListSEXP>("StaticImport");

  auto staticExports = std::make_shared<ListSEXP>("");

  auto staticStarExports = std::make_shared<ListSEXP>("StarExport");

  bool isModule = false;
  double topLevelScopeIdx = 0;

  // Iterate over BBContainerSEXP
  for (auto &bbc : fileSexp->args)
  {
    auto container = std::dynamic_pointer_cast<BBContainerSEXP>(bbc);
    assert(iridiumBuildContext.find(container->getScopeIDX()) != iridiumBuildContext.end());
    auto containerBC = iridiumBuildContext[container->getScopeIDX()];
    bool isTopLevel = container->hasTopLevel();

    if (isTopLevel)
    {
      isModule = containerBC->isModule;
      topLevelScopeIdx = containerBC->scopeIdx;
    }
    else
      continue;

    // If this is a top level module, the populate module requests
    if (containerBC->moduleRequestMap)
    {
      auto &moduleRequestMap = containerBC->moduleRequestMap.value();
      for (auto &e : moduleRequestMap)
      {
        assert(std::dynamic_pointer_cast<ModuleRequestSEXP>(e.second) && "Expected ModuleRequestSEXP");
        addToListSEXP(moduleRequests, e.second);
      }
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
        // import a from "SOURCE";
        if (auto staticImportStmt = std::dynamic_pointer_cast<StaticImportSEXP>(stmt))
        {
          assert(isTopLevel);
          auto storageTarget = std::dynamic_pointer_cast<ResolveEnvBindingSEXP>(staticImportStmt->getStorageLocation());
          assert(storageTarget && "Expected storageTarget to be ResolveEnvBindingSEXP");

          std::string bindingName = storageTarget->getNAME();

          // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
          auto binding = std::make_shared<EnvBindingSEXP>(bindingName, false, false, false, true, false, false, containerBC->scopeIdx, -1, localScope, parentClosureScope, -1);
          
          // IRISEXP ParentReference, bool NSIMPORT, double REFIDX
          auto remoteBinding = std::make_shared<RemoteEnvBindingSEXP>(binding, false, -1);

          auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(container->getBindings());
          assert(bindingsSEXP && "Expected bindingsSEXP");
          addToListSEXP(bindingsSEXP->getRemoteBindings(), remoteBinding);

          addToListSEXP(staticImports, staticImportStmt);
          bb->args.at(i) = std::make_shared<NOPSEXP>();
        }

        // export { a as b };
        if (auto localStaticExportStmt = std::dynamic_pointer_cast<LocalStaticExportSEXP>(stmt))
        {
          assert(isTopLevel);
          auto localBinding = localStaticExportStmt->getStorageLocation();
          assert(std::dynamic_pointer_cast<ResolveEnvBindingSEXP>(localBinding) && "Expected localBinding to be ResolveEnvBindingSEXP");
          addToListSEXP(staticExports, localStaticExportStmt);
          bb->args.at(i) = std::make_shared<NOPSEXP>();
        }

        // export * as foo from "SOURCE";
        if (auto namedReexportStmt = std::dynamic_pointer_cast<NamedReexportSEXP>(stmt))
        {
          assert(isTopLevel);
          addToListSEXP(staticExports, namedReexportStmt);
          bb->args.at(i) = std::make_shared<NOPSEXP>();
        }

        // export * from "SOURCE";
        if (auto starExportStmt = std::dynamic_pointer_cast<StarExportSEXP>(stmt))
        {
          assert(isTopLevel);
          addToListSEXP(staticStarExports, starExportStmt); // Creates no binding
          bb->args.at(i) = std::make_shared<NOPSEXP>();
        }
      }
    }
  }
  std::vector<IRISEXP> newArgs;
  newArgs.push_back(moduleRequests);
  newArgs.push_back(staticImports);
  newArgs.push_back(staticExports);
  newArgs.push_back(staticStarExports);
  newArgs.insert(newArgs.end(),
                 fileSexp->args.begin(),
                 fileSexp->args.end());
  fileSexp->args = std::move(newArgs);
}