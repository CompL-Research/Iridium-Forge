// Generated Stub for IRI_TAG::Unop
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Unop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Val -> sexp.getArg_Val()
 * Flags:
 *   - string OP -> sexp.getOP()
 */
void handleUnop(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case Unop");
  // IRI_GEN::UnopSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : Unop ===
  // if (sexp.hasArg_Val()) { IRID arg_Val = sexp.getArg_Val(); }
  // if (sexp.hasOP()) { StringID str_OP = sexp.getOP(); }

  return ;
}

} // namespace IRI_STRUCTURAL
