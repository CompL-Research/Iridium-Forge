// Generated Stub for IRI_TAG::PushCatchContext
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PushCatchContext
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - double IDX -> sexp.getIDX()
 */
void handlePushCatchContext(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case PushCatchContext");
  // IRI_GEN::PushCatchContextSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : PushCatchContext ===
  // if (sexp.hasIDX()) { double dbl_IDX = sexp.getIDX(); }

  return;
}

} // namespace IRI_STRUCTURAL
