// Generated Stub for IRI_TAG::Goto
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Goto
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - void   Deferred -> sexp.hasDeferred()
 *   - double IDX -> sexp.getIDX()
 */
void handleGoto(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case Goto");
  // IRI_GEN::GotoSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : Goto ===
  // bool has_Deferred = sexp.hasDeferred();
  // if (sexp.hasIDX()) { double dbl_IDX = sexp.getIDX(); }

  return;
}

} // namespace IRI_STRUCTURAL
