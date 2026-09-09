// Generated Stub for IRI_TAG::IfElseJump
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: IfElseJump
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Test -> sexp.getArg_Test()
 * Flags:
 *   - void   NOT -> sexp.hasNOT()
 *   - double TRUE -> sexp.getTRUE()
 *   - double FALSE -> sexp.getFALSE()
 */
void handleIfElseJump(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case IfElseJump");
  // IRI_GEN::IfElseJumpSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : IfElseJump ===
  // if (sexp.hasArg_Test()) { IRID arg_Test = sexp.getArg_Test(); }
  // bool has_NOT = sexp.hasNOT();
  // if (sexp.hasTRUE()) { double dbl_TRUE = sexp.getTRUE(); }
  // if (sexp.hasFALSE()) { double dbl_FALSE = sexp.getFALSE(); }

  return ;
}

} // namespace IRI_STRUCTURAL
