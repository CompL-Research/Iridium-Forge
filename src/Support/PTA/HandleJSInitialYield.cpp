// Generated Stub for IRI_TAG::JSInitialYield
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSInitialYield
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleJSInitialYield(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSInitialYield");
  // IRI_GEN::JSInitialYieldSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSInitialYield ===

  return;
}

} // namespace IRI_STRUCTURAL
