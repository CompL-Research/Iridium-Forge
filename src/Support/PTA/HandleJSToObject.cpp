// Generated Stub for IRI_TAG::JSToObject
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSToObject
 * Meta:    AMP
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 * Flags: (none)
 */
void handleJSToObject(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSToObject");
  // IRI_GEN::JSToObjectSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSToObject ===
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }

  return ;
}

} // namespace IRI_STRUCTURAL
