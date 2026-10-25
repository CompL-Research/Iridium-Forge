// Generated Stub for IRI_TAG::ReturnAsync
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ReturnAsync
 * Meta:    STMT
 * Arguments:
 *   [0] IRID RetVal -> sexp.getArg_RetVal()
 * Flags: (none)
 */
void handleReturnAsync(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case ReturnAsync");
  // IRI_GEN::ReturnAsyncSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : ReturnAsync ===
  // if (sexp.hasArg_RetVal()) { IRID arg_RetVal = sexp.getArg_RetVal(); }

  return;
}

} // namespace IRI_STRUCTURAL
