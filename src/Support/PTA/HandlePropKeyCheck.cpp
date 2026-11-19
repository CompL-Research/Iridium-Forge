// Generated Stub for IRI_TAG::PropKeyCheck
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PropKeyCheck
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void handlePropKeyCheck(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case PropKeyCheck");
  // IRI_GEN::PropKeyCheckSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : PropKeyCheck ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }

  return;
}

} // namespace IRI_STRUCTURAL
