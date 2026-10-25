

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline void patchNode(IRIContext &ctx, IRID node, double startScope) {
  auto args = ctx.storage.nodes.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    if (IRI_NODE(ctx, currID).tag == JSUnop) {
      JSUnopSEXP unop(currID, ctx);
      IRID val = unop.getArg_Val();
      if (IRI_NODE(ctx, val).tag == IRI_GEN::UNOPDelVar) {
        UNOPDelVarSEXP dv(val, ctx);
        if (!ctx.iris->isGlobal(dv.getNAME(), startScope)) {
          ctx.storage.nodes.update_arg_inplace(node, i, BooleanSEXP::create(ctx, false));
        }
      }
    }
    patchNode(ctx, args[i], startScope);
  }
}

void _18_DELOP(IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);
      auto bbScope = bb.getScopeIDX();
      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        patchNode(ctx, stmtID, bbScope);
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
