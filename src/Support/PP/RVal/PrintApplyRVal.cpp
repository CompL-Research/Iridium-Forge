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
 *   - void   ConstructorCall -> sexp.hasConstructorCall()
 *   - void   Super -> sexp.hasSuper()
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
std::string printApplyRVal(const PPContext &pp, IRID node) {
  // ctx->f(...args). ctx is `this` for plain calls and new.target for new.
  // Mirrors the lowering order:
  //   super(ctor: f, new.target: nt)(...args)
  //   new f->f(...args)       new.target = callee (ctx unused)
  //   eval#N f(...args)       no ctx
  //   ctx->f(...args)         this = ctx
  using S = IRI_GEN::ApplySEXP;
  S sexp(node, pp.ctx);
  std::string callee = ppOperand(pp, sexp.getArg_Callee());
  std::string call =
      callee + "(..." + ppOperand(pp, sexp.getArg_ArgList()) + ")";
  std::string ctx = ppOperand(pp, sexp.getArg_Context());

  if (sexp.hasSuper())
    return "super(ctor: " + ppRVal(pp, sexp.getArg_Callee()) +
           ", new.target: " + ppRVal(pp, sexp.getArg_Context()) + ")(..." +
           ppOperand(pp, sexp.getArg_ArgList()) + ")";
  if (sexp.hasConstructorCall())
    return "new " + callee + "->" + call;
  if (sexp.hasJSDirectEval())
    return "eval#" + ppNumber(sexp.getJSDirectEval()) + " " + call;
  return ctx + "->" + call;
}

} // namespace IRI_STRUCTURAL
