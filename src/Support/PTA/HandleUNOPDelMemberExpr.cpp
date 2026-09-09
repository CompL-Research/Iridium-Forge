// Generated Stub for IRI_TAG::UNOPDelMemberExpr
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: UNOPDelMemberExpr
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Receiver -> sexp.getArg_Receiver()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
void handleUNOPDelMemberExpr(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case UNOPDelMemberExpr");
  // IRI_GEN::UNOPDelMemberExprSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : UNOPDelMemberExpr ===
  // if (sexp.hasArg_Receiver()) { IRID arg_Receiver = sexp.getArg_Receiver(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }

  return ;
}

} // namespace IRI_STRUCTURAL
