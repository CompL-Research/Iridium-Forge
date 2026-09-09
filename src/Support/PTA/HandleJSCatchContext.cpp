// Generated Stub for IRI_TAG::JSCatchContext
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCatchContext
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 */
void handleJSCatchContext(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSCatchContext");
  // IRI_GEN::JSCatchContextSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSCatchContext ===
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }

  return ;
}

} // namespace IRI_STRUCTURAL
