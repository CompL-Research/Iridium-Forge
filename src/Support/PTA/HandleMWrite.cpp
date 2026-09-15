// Generated Stub for IRI_TAG::MWrite
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: MWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID LValTarget -> sexp.getArg_LValTarget()
 *   [1] IRID RVal -> sexp.getArg_RVal()
 * Flags:
 *   - void   INIT -> sexp.hasINIT()
 *   - void   SAFE -> sexp.hasSAFE()
 */
void handleMWrite(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case MWrite");
  // IRI_GEN::MWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : MWrite ===
  // if (sexp.hasArg_LValTarget()) { IRID arg_LValTarget = sexp.getArg_LValTarget(); }
  // if (sexp.hasArg_RVal()) { IRID arg_RVal = sexp.getArg_RVal(); }
  // bool has_INIT = sexp.hasINIT();
  // bool has_SAFE = sexp.hasSAFE();

  return;
}

} // namespace IRI_STRUCTURAL
