// Generated Stub for IRI_TAG::IfJump
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: IfJump
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Test -> sexp.getArg_Test()
 * Flags:
 *   - void   NOT -> sexp.hasNOT()
 *   - double IDX -> sexp.getIDX()
 */
void handleIfJump(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case IfJump");
  // IRI_GEN::IfJumpSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : IfJump ===
  // if (sexp.hasArg_Test()) { IRID arg_Test = sexp.getArg_Test(); }
  // bool has_NOT = sexp.hasNOT();
  // if (sexp.hasIDX()) { double dbl_IDX = sexp.getIDX(); }

  return ;
}

} // namespace IRI_STRUCTURAL
