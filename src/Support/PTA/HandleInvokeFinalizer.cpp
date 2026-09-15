// Generated Stub for IRI_TAG::InvokeFinalizer
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: InvokeFinalizer
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - double IDX -> sexp.getIDX()
 */
void handleInvokeFinalizer(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case InvokeFinalizer");
  // IRI_GEN::InvokeFinalizerSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : InvokeFinalizer ===
  // if (sexp.hasIDX()) { double dbl_IDX = sexp.getIDX(); }

  return;
}

} // namespace IRI_STRUCTURAL
