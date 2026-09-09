// Generated Stub for IRI_TAG::InvokeFinalizer
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: InvokeFinalizer
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - double IDX -> sexp.getIDX()
 */
void handleInvokeFinalizer(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case InvokeFinalizer");
  // IRI_GEN::InvokeFinalizerSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : InvokeFinalizer ===
  // if (sexp.hasIDX()) { double dbl_IDX = sexp.getIDX(); }

  return ;
}

} // namespace IRI_STRUCTURAL
