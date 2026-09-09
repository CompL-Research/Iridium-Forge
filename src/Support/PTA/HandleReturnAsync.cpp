// Generated Stub for IRI_TAG::ReturnAsync
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ReturnAsync
 * Meta:    STMT
 * Arguments:
 *   [0] IRID RetVal -> sexp.getArg_RetVal()
 * Flags: (none)
 */
void handleReturnAsync(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case ReturnAsync");
  // IRI_GEN::ReturnAsyncSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : ReturnAsync ===
  // if (sexp.hasArg_RetVal()) { IRID arg_RetVal = sexp.getArg_RetVal(); }

  return ;
}

} // namespace IRI_STRUCTURAL
