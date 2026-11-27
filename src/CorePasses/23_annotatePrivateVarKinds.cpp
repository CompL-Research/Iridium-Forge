#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include <memory>
#include <unordered_map>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

// Binding needs a special flag for eval to recognize it as a pvt field/setter meth/etc etc...
void _23_APVK(IRIContext &ctx, IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {
  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);
      for (auto [stmtID, _] : bb.stmts()) {
        if (IRI_NODE(ctx, stmtID).tag != IRI_GEN::LWrite)
          continue;
        LWriteSEXP write(stmtID, ctx);
        IRID lval = write.getArg_LValTarget();
        IRID rval = write.getArg_RVal();
        if (IRI_NODE(ctx, lval).tag != IRI_GEN::EnvBinding)
          continue;
        if (IRI_NODE(ctx, rval).tag != IRI_GEN::JSPrivate)
          continue;
        JSPrivateSEXP symbol(rval, ctx);
        EnvBindingSEXP binding(lval, ctx);
        if (symbol.hasPRIVFIELD())
          binding.setPRIVFIELD();
        else if (symbol.hasPRIVMETHOD())
          binding.setPRIVMETHOD();
        else if (symbol.hasPRIVGET())
          binding.setPRIVGET();
        else if (symbol.hasPRIVGETSET())
          binding.setPRIVGETSET();
        else if (symbol.hasPRIVSET())
          binding.setPRIVSET();
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
