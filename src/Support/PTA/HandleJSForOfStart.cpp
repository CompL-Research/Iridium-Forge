// Generated Stub for IRI_TAG::JSForOfStart
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForOfStart
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   AWAIT -> sexp.getAWAIT()
 */
void handleJSForOfStart(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSForOfStart");
  // IRI_GEN::JSForOfStartSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSForOfStart ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasAWAIT()) { bool val_AWAIT = sexp.getAWAIT(); }

  return;
}

} // namespace IRI_STRUCTURAL
