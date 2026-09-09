// Generated Stub for IRI_TAG::Return
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Return
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void handleReturn(const PTAStatementContext &ctx) {
  Prakriti::ECMAGraph *G = ctx.incomingState;
  IRI_GEN::ReturnSEXP sexp(ctx.stmt.id, ctx.pool);
  IRID arg = sexp.getArg_Obj();

  // Karma [[Get]]
  KarmaBindu(G,
             G->getPointees(arg, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get)),
             {NULL, {arg}});

  return;
}

} // namespace IRI_STRUCTURAL
