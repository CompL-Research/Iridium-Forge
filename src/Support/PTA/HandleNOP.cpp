// Generated Stub for IRI_TAG::NOP
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: NOP
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleNOP(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case NOP");
  // IRI_GEN::NOPSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : NOP ===

  return ;
}

} // namespace IRI_STRUCTURAL
