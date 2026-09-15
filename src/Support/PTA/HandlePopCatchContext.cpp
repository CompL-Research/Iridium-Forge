// Generated Stub for IRI_TAG::PopCatchContext
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PopCatchContext
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handlePopCatchContext(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case PopCatchContext");
  // IRI_GEN::PopCatchContextSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : PopCatchContext ===

  return;
}

} // namespace IRI_STRUCTURAL
