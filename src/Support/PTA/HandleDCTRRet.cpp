// Generated Stub for IRI_TAG::DCTRRet
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: DCTRRet
 * Meta:    AMP
 * Arguments:
 *   [0] IRID userObj -> sexp.getArg_userObj()
 *   [1] IRID thisObj -> sexp.getArg_thisObj()
 * Flags: (none)
 */
void handleDCTRRet(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case DCTRRet");
  // IRI_GEN::DCTRRetSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : DCTRRet ===
  // if (sexp.hasArg_userObj()) { IRID arg_userObj = sexp.getArg_userObj(); }
  // if (sexp.hasArg_thisObj()) { IRID arg_thisObj = sexp.getArg_thisObj(); }

  return ;
}

} // namespace IRI_STRUCTURAL
