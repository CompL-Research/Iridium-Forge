// Generated Stub for IRI_TAG::StackToHeap
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: StackToHeap
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleStackToHeap(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case StackToHeap");
  // IRI_GEN::StackToHeapSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : StackToHeap ===

  return ;
}

} // namespace IRI_STRUCTURAL
