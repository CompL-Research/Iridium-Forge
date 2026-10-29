// Generated Stub for IRI_TAG::JSDefineGetter
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSDefineGetter
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID Key -> sexp.getArg_Key()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   NOENUM -> sexp.hasNOENUM()
 */
void handleJSDefineGetter(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSDefineGetter");
  // IRI_GEN::JSDefineGetterSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSDefineGetter ===
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }
  // if (sexp.hasArg_Key()) { IRID arg_Key = sexp.getArg_Key(); }
  // if (sexp.hasArg_Value()) { IRID arg_Value = sexp.getArg_Value(); }
  // bool has_NOENUM = sexp.hasNOENUM();

  return;
}

} // namespace IRI_STRUCTURAL
