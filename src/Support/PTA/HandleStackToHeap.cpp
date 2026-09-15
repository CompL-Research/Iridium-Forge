// Generated Stub for IRI_TAG::StackToHeap
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: StackToHeap
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleStackToHeap(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case StackToHeap");
  // IRI_GEN::StackToHeapSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : StackToHeap ===

  return;
}

} // namespace IRI_STRUCTURAL
