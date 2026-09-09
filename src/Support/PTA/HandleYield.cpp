// Generated Stub for IRI_TAG::Yield
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Yield
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void handleYield(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case Yield");
  // IRI_GEN::YieldSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : Yield ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }

  return ;
}

} // namespace IRI_STRUCTURAL
