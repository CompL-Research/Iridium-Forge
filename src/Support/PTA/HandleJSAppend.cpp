// Generated Stub for IRI_TAG::JSAppend
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSAppend
 * Meta:    AMP
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID InsertionIdx -> sexp.getArg_InsertionIdx()
 *   [2] IRID SpreadObj -> sexp.getArg_SpreadObj()
 * Flags: (none)
 */
void handleJSAppend(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSAppend");
  // IRI_GEN::JSAppendSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSAppend ===
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }
  // if (sexp.hasArg_InsertionIdx()) { IRID arg_InsertionIdx = sexp.getArg_InsertionIdx(); }
  // if (sexp.hasArg_SpreadObj()) { IRID arg_SpreadObj = sexp.getArg_SpreadObj(); }

  return ;
}

} // namespace IRI_STRUCTURAL
