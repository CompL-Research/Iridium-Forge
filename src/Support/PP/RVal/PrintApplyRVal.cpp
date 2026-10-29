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
 *   - void   ConstructorCall -> pp.on(node, IRI_GEN::ApplySEXP::FLAG_IDX_ConstructorCall)
 *   - void   Super -> pp.on(node, IRI_GEN::ApplySEXP::FLAG_IDX_Super)
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
std::string printApplyRVal(const PPContext &pp, IRID node) {
  // ctx->f(...args). ctx is `this` for plain calls and new.target for
  // super/new (construct). Mirrors the lowering order:
  //   super ctx->f(...args)   new.target = ctx
  //   new f->f(...args)       new.target = callee (ctx unused)
  //   eval#N f(...args)       no ctx
  //   ctx->f(...args)         this = ctx
  using S = IRI_GEN::ApplySEXP;
  S sexp(node, pp.ctx);
  std::string callee = ppOperand(pp, sexp.getArg_Callee());
  std::string call =
      callee + "(..." + ppOperand(pp, sexp.getArg_ArgList()) + ")";
  std::string ctx = ppOperand(pp, sexp.getArg_Context());

  if (pp.on(node, S::FLAG_IDX_Super))
    return "super " + ctx + "->" + call;
  if (pp.on(node, S::FLAG_IDX_ConstructorCall))
    return "new " + callee + "->" + call;
  if (sexp.hasJSDirectEval())
    return "eval#" + ppNumber(sexp.getJSDirectEval()) + " " + call;
  return ctx + "->" + call;
}

} // namespace IRI_STRUCTURAL
