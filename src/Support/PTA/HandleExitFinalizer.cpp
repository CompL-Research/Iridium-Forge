// Generated Stub for IRI_TAG::ExitFinalizer
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ExitFinalizer
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleExitFinalizer(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case ExitFinalizer");
  // IRI_GEN::ExitFinalizerSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : ExitFinalizer ===

  return;
}

} // namespace IRI_STRUCTURAL
