// Generated Stub for IRI_TAG::JSPrivateFieldRead
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSPrivateFieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
void handleJSPrivateFieldRead(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSPrivateFieldRead");
  // IRI_GEN::JSPrivateFieldReadSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSPrivateFieldRead ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }

  return ;
}

} // namespace IRI_STRUCTURAL
