// Generated Stub for IRI_TAG::LWrite
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: LWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID LValTarget -> sexp.getArg_LValTarget()
 *   [1] IRID RVal -> sexp.getArg_RVal()
 * Flags:
 *   - void   INIT -> sexp.hasINIT()
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   THISINIT -> sexp.hasTHISINIT()
 */
void handleLWrite(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case LWrite");
  // IRI_GEN::LWriteSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : LWrite ===
  // if (sexp.hasArg_LValTarget()) { IRID arg_LValTarget = sexp.getArg_LValTarget(); }
  // if (sexp.hasArg_RVal()) { IRID arg_RVal = sexp.getArg_RVal(); }
  // bool has_INIT = sexp.hasINIT();
  // bool has_SAFE = sexp.hasSAFE();
  // bool has_THISINIT = sexp.hasTHISINIT();

  return;
}

} // namespace IRI_STRUCTURAL
