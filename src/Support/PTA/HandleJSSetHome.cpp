// Generated Stub for IRI_TAG::JSSetHome
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSetHome
 * Meta:    STMT
 * Arguments:
 *   [0] IRID HomeObj -> sexp.getArg_HomeObj()
 *   [1] IRID FuncObj -> sexp.getArg_FuncObj()
 * Flags: (none)
 */
void handleJSSetHome(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSSetHome");
  // IRI_GEN::JSSetHomeSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSSetHome ===
  // if (sexp.hasArg_HomeObj()) { IRID arg_HomeObj = sexp.getArg_HomeObj(); }
  // if (sexp.hasArg_FuncObj()) { IRID arg_FuncObj = sexp.getArg_FuncObj(); }

  return;
}

} // namespace IRI_STRUCTURAL
