// Generated Stub for IRI_TAG::JSDefineSetter
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSDefineSetter
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID Key -> sexp.getArg_Key()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   NOENUM -> sexp.hasNOENUM()
 */
void handleJSDefineSetter(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case JSDefineSetter");
  // IRI_GEN::JSDefineSetterSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : JSDefineSetter ===
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }
  // if (sexp.hasArg_Key()) { IRID arg_Key = sexp.getArg_Key(); }
  // if (sexp.hasArg_Value()) { IRID arg_Value = sexp.getArg_Value(); }
  // bool has_NOENUM = sexp.hasNOENUM();

  return;
}

} // namespace IRI_STRUCTURAL
