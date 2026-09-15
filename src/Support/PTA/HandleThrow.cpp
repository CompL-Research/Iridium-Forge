// Generated Stub for IRI_TAG::Throw
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Throw
 * Meta:    STMT
 * Arguments:
 *   [0] IRID ThrowVal -> sexp.getArg_ThrowVal()
 * Flags: (none)
 */
void handleThrow(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case Throw");
  // IRI_GEN::ThrowSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : Throw ===
  // if (sexp.hasArg_ThrowVal()) { IRID arg_ThrowVal = sexp.getArg_ThrowVal(); }

  return;
}

} // namespace IRI_STRUCTURAL
