// Generated Stub for IRI_TAG::NOP
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: NOP
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleNOP(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case NOP");
  // IRI_GEN::NOPSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : NOP ===

  return;
}

} // namespace IRI_STRUCTURAL
