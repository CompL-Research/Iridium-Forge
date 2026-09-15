// Generated Stub for IRI_TAG::Return
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Return
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void handleReturn(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case Return");
  // IRI_GEN::ReturnSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : Return ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }

  return;
}

} // namespace IRI_STRUCTURAL
