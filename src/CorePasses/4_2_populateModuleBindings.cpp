#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <cassert>
#include <memory>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _4_2_PMB(IridiumPool &pool, IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {

  pool.iris = std::make_shared<IRIS>(pool, iridiumBuildContext, fileSEXP);

  // Vectors to allocate
  FileSupport fileSupport(fileSEXP, pool);

  // Other files referenced by this module
  std::vector<IRID> moduleRequestsVec;
  auto moduleRequests = pool.get_args_view(fileSEXP)[0];

  // Objects imported by this module
  std::vector<IRID> staticImportsVec;
  auto staticImports = pool.get_args_view(fileSEXP)[1];

  // Objects exported by this module
  std::vector<IRID> staticExportsVec;
  auto staticExports = pool.get_args_view(fileSEXP)[2];

  // Reexports by this module
  std::vector<IRID> staticStarExportsVec;
  auto staticStarExports = pool.get_args_view(fileSEXP)[3];

  //
  // Get top level container and Build Context
  //
  auto container =
      BBContainerSEXP(IRI_HELPERS::getTopLevelContainer(pool, fileSEXP), pool);
  auto containerBC = iridiumBuildContext[container.getScopeIDX()];

  if (containerBC->moduleRequestMap) {
    auto &moduleRequestMap = containerBC->moduleRequestMap.value();
    for (auto &e : moduleRequestMap) {
      ModuleRequestSEXP m(e.second, pool);
      moduleRequestsVec.push_back(e.second);
    }
  }

  auto bbs = pool.get_args(container.getArg_BB());

  for (auto &bbIDX : bbs) {
    BBSEXP bb(bbIDX, pool);
    auto localScope = bb.getScopeIDX();
    auto stmts = pool.get_args(bbIDX);

    for (size_t i = 0; i < stmts.size(); i++) {
      IRID stmtID = stmts[i];
      IRI_TAG currTag = pool[stmtID].tag;
      // import "SOURCE";
      // import a from "SOURCE";
      // import {a} from "SOURCE";
      // import * as foo from "SOURCE";
      if (currTag == IRI_GEN::StaticImport) {
        StaticImportSEXP staticImportStmt(stmtID, pool);
        ResolveEnvBindingSEXP storageTarget(
            staticImportStmt.getArg_StorageLocation(), pool);
        StringID bindingName = storageTarget.getNAME();

        assert(localScope == pool.iris->getTopLevelScope());

        IRID store = pool.iris->declareRBinding(localScope, bindingName, IRI_GEN::JSCONST, staticImportStmt.hasNSIMPORT() ? MODULENSI : MODULEI).ID;
        staticImportStmt.setArg_StorageLocation(store);

        staticImportsVec.push_back(stmtID);
        pool.update_arg_inplace(bbIDX, i, pool.NOP_SEXP);
      }
      // export { a as b };
      else if (currTag == IRI_GEN::LocalStaticExport) {
        LocalStaticExportSEXP localStaticExportStmt(stmtID, pool);
        ResolveEnvBindingSEXP localBinding(
            localStaticExportStmt.getArg_StorageLocation(), pool);

        assert(localScope == pool.iris->getTopLevelScope());
        pool.iris->ensureExportedBindingIsModuleBinding(localBinding.getNAME());

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
}
} // namespace IRI_CORE_PASSES
