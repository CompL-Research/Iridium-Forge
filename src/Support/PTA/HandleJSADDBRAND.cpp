// Generated Stub for IRI_TAG::JSADDBRAND
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSADDBRAND
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID HomeObj -> sexp.getArg_HomeObj()
 * Flags: (none)
 */
void handleJSADDBRAND(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSADDBRAND");
  // IRI_GEN::JSADDBRANDSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSADDBRAND ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_HomeObj()) { IRID arg_HomeObj = sexp.getArg_HomeObj(); }

  return;
}

} // namespace IRI_STRUCTURAL
