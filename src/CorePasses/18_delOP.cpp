

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

namespace {

IRID resolveDelete(IRIContext &ctx, IRID node, SCPIDX startScope) {
  IRI_TAG tag = IRI_NODE(ctx, node).tag;
  if (tag != JSUnop && tag != Unop)
    return node;

  IRID val = tag == JSUnop ? JSUnopSEXP(node, ctx).getArg_Val()
                           : UnopSEXP(node, ctx).getArg_Val();
  if (IRI_NODE(ctx, val).tag != IRI_GEN::UNOPDelVar)
    return node;

  UNOPDelVarSEXP dv(val, ctx);
  if (!ctx.iris->isGlobal(dv.getNAME(), startScope))
    return BooleanSEXP::create(ctx, false);

  dv.setGLOBAL();
  return node;
}

void patchNode(IRIContext &ctx, IRID node, SCPIDX startScope) {
  auto args = ctx.storage.nodes.get_args(node);

  for (size_t i = 0; i < args.size(); i++) {
    IRID patched = resolveDelete(ctx, args[i], startScope);
    if (patched != args[i])
      ctx.storage.nodes.update_arg_inplace(node, i, patched);
    patchNode(ctx, patched, startScope);
  }
}

} // namespace

void _18_DELOP(IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID fileSEXP,
               BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);
      auto bbScope = bb.getScopeIDX();
      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        IRID root = resolveDelete(ctx, stmtID, bbScope);
        if (root != stmtID)
          ctx.storage.nodes.update_arg_inplace(bbID, stmtOffset, root);
        patchNode(ctx, root, bbScope);
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
