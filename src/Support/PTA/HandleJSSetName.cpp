// Generated Stub for IRI_TAG::JSSetName
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSetName
 * Meta:    STMT
 * Arguments:
 *   [0] IRID obj -> sexp.getArg_obj()
 *   [1] IRID name -> sexp.getArg_name()
 * Flags: (none)
 */
void handleJSSetName(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSSetName");
  // IRI_GEN::JSSetNameSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSSetName ===
  // if (sexp.hasArg_obj()) { IRID arg_obj = sexp.getArg_obj(); }
  // if (sexp.hasArg_name()) { IRID arg_name = sexp.getArg_name(); }

  return;
}

} // namespace IRI_STRUCTURAL
