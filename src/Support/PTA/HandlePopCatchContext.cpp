// Generated Stub for IRI_TAG::PopCatchContext
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PopCatchContext
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handlePopCatchContext(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case PopCatchContext");
  // IRI_GEN::PopCatchContextSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : PopCatchContext ===

  return ;
}

} // namespace IRI_STRUCTURAL
