#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _4_2_PMB(IridiumPool &pool, IRID fileSEXP, BUILD_CTX &iridiumBuildContext) {


  // Vectors to allocate

  // Other files referenced by this module
  std::vector<IRID> moduleRequestsVec;
  auto moduleRequests =
      ListSEXP::create(pool, pool.strings.intern("ModuleRequest"));

  // Objects imported by this module
  std::vector<IRID> staticImportsVec;
  auto staticImports =
      ListSEXP::create(pool, pool.strings.intern("StaticImport"));

  // Objects exported by this module
  std::vector<IRID> staticExportsVec;
  auto staticExports = ListSEXP::create(pool, pool.strings.intern(""));

  // Reexports by this module
  std::vector<IRID> staticStarExportsVec;
  auto staticStarExports =
      ListSEXP::create(pool, pool.strings.intern("StarExport"));

  //
  // Get top level container and Build Context
  //
  auto container =
      BBContainerSEXP(IRI_HELPERS::getTopLevelContainer(pool, fileSEXP), pool);
  auto containerBC = iridiumBuildContext[container.getScopeIDX()];
  bool isTopLevel = container.hasTopLevel();

  if (containerBC->moduleRequestMap) {
    auto &moduleRequestMap = containerBC->moduleRequestMap.value();
    for (auto &e : moduleRequestMap) {
      ModuleRequestSEXP m(e.second, pool);
      moduleRequestsVec.push_back(e.second);
    }
  }


  auto bbs = pool.get_args(container.getArg_BB());


  //
  // Bindings added to top level scope by the import statements
  //
  std::vector<IRID> remoteBindingsVector;
  BindingsSEXP bindings(container.getArg_Bindings(), pool);
  ListSEXP remoteBindings(bindings.getArg_RemoteBindings(), pool);


  for (auto &bbIDX : bbs) {
    BBSEXP bb(bbIDX, pool);
    auto localScope = bb.getScopeIDX();
    auto parentClosureScope = IRI_HELPERS::findParentClosureScope(
        pool, localScope, iridiumBuildContext);
    auto stmts = pool.get_args(bbIDX);


    for (size_t i = 0; i < stmts.size(); i++) {
      IRID stmtID = stmts[i];
      IRI_TAG currTag = pool[stmtID].tag;
      // import a from "SOURCE";
      if (currTag == IRI_GEN::StaticImport) {
        StaticImportSEXP staticImportStmt(stmtID, pool);
        ResolveEnvBindingSEXP storageTarget(staticImportStmt.getArg_StorageLocation(), pool);
        StringID bindingName = storageTarget.getNAME();
        IRID binding = EnvBindingSEXP::create(pool, bindingName, false, false, false, true, false, false, containerBC->scopeIDX, -1, localScope, parentClosureScope, -1);
        IRID remoteBinding = RemoteEnvBindingSEXP::create(pool, binding, false, -1);
        remoteBindingsVector.push_back(remoteBinding);
        staticImportsVec.push_back(stmtID);
        pool.update_arg_inplace(bbIDX, i, pool.NOP_SEXP);
      }
      // export { a as b };
      else if (currTag == IRI_GEN::LocalStaticExport) {
        LocalStaticExportSEXP localStaticExportStmt(stmtID, pool);
        ResolveEnvBindingSEXP localBinding(localStaticExportStmt.getArg_StorageLocation(), pool);
        staticExportsVec.push_back(stmtID);
        pool.update_arg_inplace(bbIDX, i, pool.NOP_SEXP);
      }
      // export * as foo from "SOURCE"
      else if (currTag == IRI_GEN::NamedReexport) {
        NamedReexportSEXP namedReexportStmt(stmtID, pool);
        staticExportsVec.push_back(stmtID);
        pool.update_arg_inplace(bbIDX, i, pool.NOP_SEXP);
      }
      // export * from "SOURCE";
      else if (currTag == IRI_GEN::StarExport) {
        StarExportSEXP starExportStmt(stmtID, pool);
        staticStarExportsVec.push_back(stmtID);
        pool.update_arg_inplace(bbIDX, i, pool.NOP_SEXP);
      }
    }
  }

  pool.set_args(moduleRequests, moduleRequestsVec);
  pool.set_args(staticImports, staticImportsVec);
  pool.set_args(staticExports, staticExportsVec);
  pool.set_args(staticStarExports, staticStarExportsVec);
  pool.add_args_to_beginning(fileSEXP, { moduleRequests, staticImports, staticExports, staticStarExports });
  pool.set_args(remoteBindings.id, remoteBindingsVector);
}
} // namespace IRI_CORE_PASSES
