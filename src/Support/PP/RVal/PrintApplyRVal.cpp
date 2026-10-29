// Generated Stub for IRI_TAG::Apply (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Apply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID Context -> sexp.getArg_Context()
 *   [2] IRID ArgList -> sexp.getArg_ArgList()
 * Flags:
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
std::string printApplyRVal(const PPContext &pp, IRID node) {
  // ctx->f(...args), ctx is `this`. Mirrors the lowering order:
  //   eval#N f(...args)       no ctx
  //   ctx->f(...args)         this = ctx
  using S = IRI_GEN::ApplySEXP;
  S sexp(node, pp.ctx);
  std::string callee = ppOperand(pp, sexp.getArg_Callee());
  std::string call =
      callee + "(..." + ppOperand(pp, sexp.getArg_ArgList()) + ")";
  std::string ctx = ppOperand(pp, sexp.getArg_Context());

  if (sexp.hasJSDirectEval())
    return "eval#" + ppNumber(sexp.getJSDirectEval()) + " " + call;
  return ctx + "->" + call;
}

} // namespace IRI_STRUCTURAL
