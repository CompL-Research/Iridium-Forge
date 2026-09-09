// Generated Stub for IRI_TAG::JSComputedFieldRead
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSComputedFieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 */
void handleJSComputedFieldRead(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSComputedFieldRead");
  // IRI_GEN::JSComputedFieldReadSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSComputedFieldRead ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }
  // bool has_SAFE = sexp.hasSAFE();

  return ;
}

} // namespace IRI_STRUCTURAL
