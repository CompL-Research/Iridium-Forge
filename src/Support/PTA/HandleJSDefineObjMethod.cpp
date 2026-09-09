// Generated Stub for IRI_TAG::JSDefineObjMethod
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSDefineObjMethod
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID Key -> sexp.getArg_Key()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   NOENUM -> sexp.hasNOENUM()
 *   - void   METHOD -> sexp.hasMETHOD()
 *   - void   GET -> sexp.hasGET()
 *   - void   SET -> sexp.hasSET()
 */
void handleJSDefineObjMethod(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSDefineObjMethod");
  // IRI_GEN::JSDefineObjMethodSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSDefineObjMethod ===
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }
  // if (sexp.hasArg_Key()) { IRID arg_Key = sexp.getArg_Key(); }
  // if (sexp.hasArg_Value()) { IRID arg_Value = sexp.getArg_Value(); }
  // bool has_NOENUM = sexp.hasNOENUM();
  // bool has_METHOD = sexp.hasMETHOD();
  // bool has_GET = sexp.hasGET();
  // bool has_SET = sexp.hasSET();

  return ;
}

} // namespace IRI_STRUCTURAL
