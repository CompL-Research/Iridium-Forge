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

void _4_2_PMB(IRIContext &ctx, IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {

  ctx.iris = std::make_shared<IRIS>(ctx, iridiumBuildContext, fileSEXP);

  // Vectors to allocate
  FileSupport fileSupport(fileSEXP, ctx);

  // Other files referenced by this module
  std::vector<IRID> moduleRequestsVec;
  auto moduleRequests = ctx.storage.nodes.get_args_view(fileSEXP)[0];

  // Objects imported by this module
  std::vector<IRID> staticImportsVec;
  auto staticImports = ctx.storage.nodes.get_args_view(fileSEXP)[1];

  // Objects exported by this module
  std::vector<IRID> staticExportsVec;
  auto staticExports = ctx.storage.nodes.get_args_view(fileSEXP)[2];

  // Reexports by this module
  std::vector<IRID> staticStarExportsVec;
  auto staticStarExports = ctx.storage.nodes.get_args_view(fileSEXP)[3];

  //
  // Get top level container and Build Context
  //
  auto container =
      BBContainerSEXP(IRI_HELPERS::getTopLevelContainer(ctx, fileSEXP), ctx);
  auto containerBC = iridiumBuildContext[container.getScopeIDX()];

  if (containerBC->moduleRequestMap) {
    auto &moduleRequestMap = containerBC->moduleRequestMap.value();
    for (auto &e : moduleRequestMap) {
      ModuleRequestSEXP m(e.second, ctx);
      moduleRequestsVec.push_back(e.second);
    }
  }

  auto bbs = ctx.storage.nodes.get_args(container.getArg_BB());

  for (auto &bbIDX : bbs) {
    BBSEXP bb(bbIDX, ctx);
    auto localScope = bb.getScopeIDX();
    auto stmts = ctx.storage.nodes.get_args(bbIDX);

    for (size_t i = 0; i < stmts.size(); i++) {
      IRID stmtID = stmts[i];
      IRI_TAG currTag = IRI_NODE(ctx, stmtID).tag;
      // import "SOURCE";
      // import a from "SOURCE";
      // import {a} from "SOURCE";
      // import * as foo from "SOURCE";
      if (currTag == IRI_GEN::StaticImport) {
        StaticImportSEXP staticImportStmt(stmtID, ctx);
        ResolveEnvBindingSEXP storageTarget(
            staticImportStmt.getArg_StorageLocation(), ctx);
        StringID bindingName = storageTarget.getNAME();

        assert(localScope == ctx.iris->getTopLevelScope());

        IRID store = ctx.iris->declareRBinding(localScope, bindingName, IRI_GEN::JSCONST, staticImportStmt.hasNSIMPORT() ? MODULENSI : MODULEI).ID;
        staticImportStmt.setArg_StorageLocation(store);

        staticImportsVec.push_back(stmtID);
        ctx.storage.nodes.update_arg_inplace(bbIDX, i, ctx.storage.nodes.NOP_SEXP);
      }
      // export { a as b };
      else if (currTag == IRI_GEN::LocalStaticExport) {
        LocalStaticExportSEXP localStaticExportStmt(stmtID, ctx);
        ResolveEnvBindingSEXP localBinding(
            localStaticExportStmt.getArg_StorageLocation(), ctx);

        assert(localScope == ctx.iris->getTopLevelScope());
        ctx.iris->ensureExportedBindingIsModuleBinding(localBinding.getNAME());

        staticExportsVec.push_back(stmtID);
        ctx.storage.nodes.update_arg_inplace(bbIDX, i, ctx.storage.nodes.NOP_SEXP);
      }
      // export * as foo from "SOURCE"
      else if (currTag == IRI_GEN::NamedReexport) {
        NamedReexportSEXP namedReexportStmt(stmtID, ctx);
        staticExportsVec.push_back(stmtID);
        ctx.storage.nodes.update_arg_inplace(bbIDX, i, ctx.storage.nodes.NOP_SEXP);
      }
      // export * from "SOURCE";
      else if (currTag == IRI_GEN::StarExport) {
        StarExportSEXP starExportStmt(stmtID, ctx);
        staticStarExportsVec.push_back(stmtID);
        ctx.storage.nodes.update_arg_inplace(bbIDX, i, ctx.storage.nodes.NOP_SEXP);
      }
    }
  }

  ctx.storage.nodes.set_args(moduleRequests, moduleRequestsVec);
  ctx.storage.nodes.set_args(staticImports, staticImportsVec);
  ctx.storage.nodes.set_args(staticExports, staticExportsVec);
  ctx.storage.nodes.set_args(staticStarExports, staticStarExportsVec);
}
} // namespace IRI_CORE_PASSES
