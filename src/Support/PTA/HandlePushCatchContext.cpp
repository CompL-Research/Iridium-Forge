// Generated Stub for IRI_TAG::PushCatchContext
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PushCatchContext
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - double IDX -> sexp.getIDX()
 */
void handlePushCatchContext(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case PushCatchContext");
  // IRI_GEN::PushCatchContextSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : PushCatchContext ===
  // if (sexp.hasIDX()) { double dbl_IDX = sexp.getIDX(); }

  return ;
}

} // namespace IRI_STRUCTURAL
