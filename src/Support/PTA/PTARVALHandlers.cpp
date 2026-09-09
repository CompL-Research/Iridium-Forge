#include "Generated/IridiumTypes.h"
#include "Storage/IridiumPool.h"
#include "Support/PTA/PTAContext.hpp"
#include "external/Prakriti.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

void handlePKRRVals(const PTAStatementContext &ctx, IRID rval,
                    std::set<Prakriti::NodeUID> &res_) {
  Prakriti::ECMAGraph *G = ctx.incomingState;
  auto &pool = ctx.pool;
  auto tag = pool[rval].tag;
  switch (tag) {
  case IRI_GEN::IRI_TAG::EnvRead: {
    EnvReadSEXP er(rval, pool);
    return handlePKRRVals(ctx, er.getArg_Obj(), res_);
    break;
  }
  case IRI_GEN::IRI_TAG::GlobalBinding: {
    GlobalBindingSEXP gb(rval, pool);
    Prakriti::NodeUID stk_ref =
        Prakriti::PKRGlobalState::getGlobal(gb.getNAME());
    auto res = KarmaBindu(
        G,
        G->getPointees(stk_ref, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get)),
        {NULL, {stk_ref}});
    for (auto &r : res) {
      res_.insert(r);
    }
    break;
  }
  case IRI_GEN::IRI_TAG::JSCTX: {
    // The closure context will provide these values.
    // Should we make these sentinals over the curr closure obj ctx?
    // CTX ::  ctx.getBB()->closure
    // 0 -> ARGUMENTS
    // 1 -> MAPPED_ARGUMENTS
    // 2 -> THIS_FUNC
    // 3 -> NEW_TARGET
    // 4 -> HOME_OBJECT
    // 5 -> VAR_OBJECT
    // 6 -> IMPORT_META
    // 7 -> GET_SUPER } contextual
    // 8 -> GET_SUPER } contextual
    // 9 -> THIS    } init for super constructor case
  }
  default:
    throw std::runtime_error("handlePKRRVals, case unhandled: " +
                             dump_tag(tag));
  }
}
} // namespace IRI_STRUCTURAL
