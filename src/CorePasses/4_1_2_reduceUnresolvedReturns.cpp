#include "Config.h"
#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include <cstddef>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _4_1_2_RUR(IRIContext &ctx, IRID fileID,
                BUILD_CTX &iridiumBuildContext) {
  StringID undefined_str = ctx.storage.strings.intern("undefined");
  StringID this_str = ctx.storage.strings.intern("this");
  FileSupport fileSupport(fileID, ctx);
  for (auto [bbContID, _] : fileSupport.containers()) {
    BBContainerSupport container(bbContID, ctx);

    bool returnThis = container.getContainerFlagID() == CF_DERIVED_CTR;

    for (auto [bbID, _] : container.bbs()) {
      BBSupport bb(bbID, ctx);

      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        IRI_TAG currTag = IRI_NODE(ctx, stmtID).tag;

        if (currTag == IRI_GEN::UnresolvedReturn) {
          IRID patched;
          if (returnThis) {
            patched = ReturnSEXP::create(
                ctx, IRI_HELPERS::createUnsafeEnvReadSEXP(ctx, this_str));
          } else {
            patched = ReturnSEXP::create(
                ctx, IRI_HELPERS::createUnsafeEnvReadSEXP(ctx, undefined_str));
          }
          ctx.storage.nodes.update_arg_inplace(bbID, stmtOffset, patched);
        } else if (currTag == IRI_GEN::Return) {
          //
          // Decorate Return statements of derived class constructors
          //
          if (returnThis) {
            ReturnSEXP rSEXP(stmtID, ctx);
            ctx.storage.nodes.update_arg_inplace(bbID, stmtOffset,
              ReturnSEXP::create(
                  ctx,
                  DCTRRetSEXP::create(ctx, rSEXP.getArg_Obj(), IRI_HELPERS::createUnsafeEnvReadSEXP(ctx, this_str))
              )
            );
          }
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
