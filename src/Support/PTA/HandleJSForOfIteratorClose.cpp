// Generated Stub for IRI_TAG::JSForOfIteratorClose
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForOfIteratorClose
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleJSForOfIteratorClose(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSForOfIteratorClose");
  // IRI_GEN::JSForOfIteratorCloseSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSForOfIteratorClose ===

  return;
}

} // namespace IRI_STRUCTURAL
