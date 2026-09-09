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

void _4_3_PIB(IridiumPool &pool, IRID fileID, BUILD_CTX &iridiumBuildContext) {
  StringID str_arguments = pool.strings.intern("arguments");
  FileSEXP fileSEXP(fileID, pool);

  StringID str_undefined = pool.strings.intern("undefined");

  auto args = pool.get_args(fileID);

  for (auto &bbcID : args) {
    if (pool[bbcID].tag != IRI_GEN::BBContainer)
      continue;

    BBContainerSEXP container(bbcID, pool);

    auto &containerBC = iridiumBuildContext[container.getScopeIDX()];

    StringID str_closure = pool.strings.intern(containerBC->name);

    auto bbs = pool.get_args(container.getArg_BB());
    for (auto &bbID : bbs) {
      BBSEXP bb(bbID, pool);

      auto localScope = bb.getScopeIDX();
      auto parentScope = iridiumBuildContext[localScope]->parent;

      auto stmts = pool.get_args(bbID);
      for (size_t i = 0; i < stmts.size(); i++) {
        IRID stmtID = stmts[i];
        IRI_TAG currTag = pool[stmtID].tag;
        if (currTag == IRI_GEN::JSImplicitBindingDeclaration) {
          JSImplicitBindingDeclarationSEXP ibs(stmtID, pool);

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

          if (pool.iris->hasBinding(ibs.getNAME(), localScope)) {
            if (ibs.getNAME() == str_arguments || ibs.getNAME() == str_closure) {
              pool.update_arg_inplace(bbID, i, pool.NOP_SEXP);
            } else {
              throw std::runtime_error("unexpected conflict when declaring implicit bindings");
            }
            continue;
          }

          auto & resolved =  pool.iris->declareLBinding(localScope, ibs.getNAME(), KIND);
          IRID lval = resolved.ID;
          IRID rval;

          double OPID = ibs.getOPID();
          if (OPID == 10) {
            rval = pool.NUBD_SEXP;
          } else if (OPID == 11) {
            rval = IRI_HELPERS::createUnsafeEnvReadSEXP(pool, str_undefined);
          } else if (OPID == 12) {
            resolved.isIMPLICITOVERRIDEABLE = true;
            rval = JSCTXSEXP::create(pool, 2);
            pool.set_args(rval, pool.get_args(ibs.getArg_Args()));
          } else {
            if (OPID < 2) {
              resolved.isIMPLICITOVERRIDEABLE = true;
            }
            rval = JSCTXSEXP::create(pool, ibs.getOPID());
            pool.set_args(rval, pool.get_args(ibs.getArg_Args()));
          }

          IRID target = LWriteSEXP::create(pool, lval, rval, true, false, OPID == 10);
          pool.update_arg_inplace(bbID, i, target);
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
