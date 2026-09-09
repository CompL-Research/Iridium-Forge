// Generated Stub for IRI_TAG::Await
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Await
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void handleAwait(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case Await");
  // IRI_GEN::AwaitSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : Await ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }

  return ;
}

} // namespace IRI_STRUCTURAL
