

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/BindingsPool.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <stdexcept>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

namespace {

IRID illegalConstWrite(IRIContext &ctx, IRID write) {
  IRID target;
  bool safe, thisInit;
  switch (IRI_NODE(ctx, write).tag) {
  case LWrite: {
    LWriteSEXP w(write, ctx);
    target = w.getArg_LValTarget();
    safe = w.hasSAFE();
    thisInit = w.hasTHISINIT();
    break;
  }
  case RWrite: {
    RWriteSEXP w(write, ctx);
    target = IRI_HELPERS::resolveRemoteBinding(ctx, w.getArg_LValTarget());
    safe = w.hasSAFE();
    thisInit = w.hasTHISINIT();
    break;
  }
  case MWrite: {
    MWriteSEXP w(write, ctx);
    target = IRI_HELPERS::resolveRemoteBinding(ctx, w.getArg_LValTarget());
    safe = w.hasSAFE();
    thisInit = false;
    break;
  }
  default:
    return NO_IRID;
  }

  if (safe || thisInit)
    return NO_IRID;

  return EnvBindingSEXP(target, ctx).hasJSCONST() ? target : NO_IRID;
}

void patchBB(IRIContext &ctx, IRID bbID) {
  BBSupport bb(bbID, ctx);
  auto oldStmts = bb.stmtsVec();

  std::vector<IRID> newStmts;
  newStmts.reserve(oldStmts.size());
  bool changed = false;

  for (IRID stmt : oldStmts) {
    IRID binding = illegalConstWrite(ctx, stmt);
    if (binding == NO_IRID) {
      if (IRI_NODE(ctx, stmt).tag == CompoundAssn) {
        // This cannot be allowed inside a CompoundAssn, we'll have to deal with
        // stack ops otherwise Nothing currently can produce such code anyway,
        // so just an assertion for now.
        // ~Meetesh
        for (IRID arg : ctx.storage.nodes.get_args_view(stmt))
          if (illegalConstWrite(ctx, arg) != NO_IRID)
            throw std::runtime_error(
                "_24_RCW: CompoundAssn writes to a const binding");
      }
      newStmts.push_back(stmt);
      continue;
    }

    changed = true;
    IRID rval = ctx.storage.nodes.get_args(stmt)[1];
    bool ignored = IRI_BINDING(ctx.iris, binding).sloppyConst;
    bool pure = get_meta(IRI_NODE(ctx, rval).tag) == RVAL;

    //
    // There are basically two cases:
    // const a;
    // ...
    // a = RVAL;
    //
    // Case 1:
    //   a is a SLOPPYCONST
    //
    //   in this case, the write is basicallt just a NOOP
    //
    //   so reduction is:
    //
    //   RVAL (if RVAL is an AMP, otherwise this is also NOOP)
    //   NOOP
    //
    //  Case 2:
    //    a is a normal CONST
    //
    //    in this case the write is basically an error
    //
    //    so the reduction is:
    //
    //   RVAL (if RVAL is an AMP, otherwise this is also NOOP)
    //   JSThrowError
    //
    if (!pure)
      newStmts.push_back(rval);
    if (!ignored)
      newStmts.push_back(JSThrowErrorSEXP::create(
          ctx, EnvBindingSEXP(binding, ctx).getNAME()));
  }

  if (changed)
    ctx.storage.nodes.set_args(bbID, newStmts);
}

} // namespace

void _24_RCW(IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID fileSEXP,
             BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    for (auto [bbID, _] : bbc.bbs())
      patchBB(ctx, bbID);
  }
}
} // namespace IRI_CORE_PASSES
