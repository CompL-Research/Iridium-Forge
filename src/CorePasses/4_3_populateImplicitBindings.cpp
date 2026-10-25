#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Support/IRIS.hpp"
#include <stdexcept>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _4_3_PIB(IRIContext &ctx, IRID fileID, BUILD_CTX &iridiumBuildContext) {
  StringID str_arguments = ctx.storage.strings.intern("arguments");
  FileSEXP fileSEXP(fileID, ctx);

  StringID str_undefined = ctx.storage.strings.intern("undefined");

  auto args = ctx.storage.nodes.get_args(fileID);

  for (auto &bbcID : args) {
    if (IRI_NODE(ctx, bbcID).tag != IRI_GEN::BBContainer)
      continue;

    BBContainerSEXP container(bbcID, ctx);

    auto &containerBC = iridiumBuildContext[container.getScopeIDX()];

    StringID str_closure = ctx.storage.strings.intern(containerBC->name);

    auto bbs = ctx.storage.nodes.get_args(container.getArg_BB());
    for (auto &bbID : bbs) {
      BBSEXP bb(bbID, ctx);

      auto localScope = bb.getScopeIDX();
      auto parentScope = iridiumBuildContext[localScope]->parent;

      auto stmts = ctx.storage.nodes.get_args(bbID);
      for (size_t i = 0; i < stmts.size(); i++) {
        IRID stmtID = stmts[i];
        IRI_TAG currTag = IRI_NODE(ctx, stmtID).tag;
        if (currTag == IRI_GEN::JSImplicitBindingDeclaration) {
          JSImplicitBindingDeclarationSEXP ibs(stmtID, ctx);

          IRI_FLAG KIND;
          if (ibs.hasJSLET()) {
            KIND = IRI_GEN::JSLET;
          } else if (ibs.hasJSCONST()) {
            KIND = IRI_GEN::JSCONST;
          } else if (ibs.hasJSVAR()) {
            KIND = IRI_GEN::JSVAR;
          } else {
            throw std::runtime_error(
                "[Forge]: Invalid kind for an implicit binding");
          }

          if (ctx.iris->hasBinding(ibs.getNAME(), localScope)) {
            if (ibs.getNAME() == str_arguments || ibs.getNAME() == str_closure) {
              ctx.storage.nodes.update_arg_inplace(bbID, i, ctx.storage.nodes.NOP_SEXP);
            } else {
              throw std::runtime_error("unexpected conflict when declaring implicit bindings");
            }
            continue;
          }

          auto & resolved =  ctx.iris->declareLBinding(localScope, ibs.getNAME(), KIND);
          IRID lval = resolved.ID;
          IRID rval;

          double OPID = ibs.getOPID();
          if (OPID == 10) {
            rval = ctx.storage.nodes.NUBD_SEXP;
          } else if (OPID == 11) {
            rval = IRI_HELPERS::createUnsafeEnvReadSEXP(ctx, str_undefined);
          } else if (OPID == 12) {
            resolved.isIMPLICITOVERRIDEABLE = true;
            rval = JSCTXSEXP::create(ctx, 2);
            ctx.storage.nodes.set_args(rval, ctx.storage.nodes.get_args(ibs.getArg_Args()));
          } else {
            if (OPID < 2) {
              resolved.isIMPLICITOVERRIDEABLE = true;
            }
            rval = JSCTXSEXP::create(ctx, ibs.getOPID());
            ctx.storage.nodes.set_args(rval, ctx.storage.nodes.get_args(ibs.getArg_Args()));
          }

          IRID target = LWriteSEXP::create(ctx, lval, rval, true, false, OPID == 10);
          ctx.storage.nodes.update_arg_inplace(bbID, i, target);
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
