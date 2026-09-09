// Generated Stub for IRI_TAG::ToNumeric
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ToNumeric
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void handleToNumeric(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case ToNumeric");
  // IRI_GEN::ToNumericSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : ToNumeric ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }

  return ;
}

} // namespace IRI_STRUCTURAL
