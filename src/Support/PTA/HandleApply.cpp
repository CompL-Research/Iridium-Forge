// Generated Stub for IRI_TAG::Apply
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Apply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID Context -> sexp.getArg_Context()
 *   [2] IRID ArgList -> sexp.getArg_ArgList()
 * Flags:
 *   - void   ConstructorCall -> sexp.hasConstructorCall()
 *   - void   Super -> sexp.hasSuper()
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
void handleApply(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case Apply");
  // IRI_GEN::ApplySEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : Apply ===
  // if (sexp.hasArg_Callee()) { IRID arg_Callee = sexp.getArg_Callee(); }
  // if (sexp.hasArg_Context()) { IRID arg_Context = sexp.getArg_Context(); }
  // if (sexp.hasArg_ArgList()) { IRID arg_ArgList = sexp.getArg_ArgList(); }
  // bool has_ConstructorCall = sexp.hasConstructorCall();
  // bool has_Super = sexp.hasSuper();
  // if (sexp.hasJSDirectEval()) { double dbl_JSDirectEval = sexp.getJSDirectEval(); }

  return ;
}

} // namespace IRI_STRUCTURAL
