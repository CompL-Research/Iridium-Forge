// Generated Stub for IRI_TAG::JSUnop
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSUnop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Val -> sexp.getArg_Val()
 * Flags:
 *   - string OP -> sexp.getOP()
 */
void handleJSUnop(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSUnop");
  // IRI_GEN::JSUnopSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSUnop ===
  // if (sexp.hasArg_Val()) { IRID arg_Val = sexp.getArg_Val(); }
  // if (sexp.hasOP()) { StringID str_OP = sexp.getOP(); }

  return ;
}

} // namespace IRI_STRUCTURAL
