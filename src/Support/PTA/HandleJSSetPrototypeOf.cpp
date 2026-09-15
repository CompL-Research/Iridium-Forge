// Generated Stub for IRI_TAG::JSSetPrototypeOf
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSetPrototypeOf
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID ProtoValue -> sexp.getArg_ProtoValue()
 * Flags: (none)
 */
void handleJSSetPrototypeOf(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSSetPrototypeOf");
  // IRI_GEN::JSSetPrototypeOfSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSSetPrototypeOf ===
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }
  // if (sexp.hasArg_ProtoValue()) { IRID arg_ProtoValue = sexp.getArg_ProtoValue(); }

  return;
}

} // namespace IRI_STRUCTURAL
