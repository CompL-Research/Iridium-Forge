// Generated Stub for IRI_TAG::PopFinalizerReturnTarget
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PopFinalizerReturnTarget
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handlePopFinalizerReturnTarget(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case PopFinalizerReturnTarget");
  // IRI_GEN::PopFinalizerReturnTargetSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : PopFinalizerReturnTarget ===

  return;
}

} // namespace IRI_STRUCTURAL
