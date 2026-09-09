// Generated Stub for IRI_TAG::Ret
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Ret
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleRet(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case Ret");
  // IRI_GEN::RetSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : Ret ===

  return ;
}

} // namespace IRI_STRUCTURAL
