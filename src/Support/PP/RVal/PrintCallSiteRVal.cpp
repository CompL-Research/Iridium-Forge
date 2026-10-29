// Generated Stub for IRI_TAG::CallSite (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: CallSite
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - void   CCall -> sexp.hasCCall()
 *   - void   PrivateCall -> sexp.hasPrivateCall()
 *   - void   Import -> sexp.hasImport()
 *   - void   V8Intrinsic -> sexp.hasV8Intrinsic()
 *   - void   TAILCALL -> sexp.hasTAILCALL()
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
std::string printCallSiteRVal(const PPContext &pp, IRID node) {
  // ctx->f(a, b), same shape as Apply; ctx is `this`. Mirrors the lowering order:
  //   eval#N f(a, b)            [f, args...], no ctx
  //   ctx->f(a, b)              CCall: [ctx, f, args...], this = ctx
  //   private ctx->f(a, b)      [ctx, f, args...], brand-checked
  //   f(a, b)                   [f, args...], no ctx
  using S = IRI_GEN::CallSiteSEXP;
  S sexp(node, pp.ctx);
  auto a = pp.args(node);
  bool eval = sexp.hasJSDirectEval();
  bool ccall = sexp.hasCCall();
  bool priv = sexp.hasPrivateCall();
  size_t calleeIdx = (ccall || priv) ? 1 : 0;
  if (a.size() <= calleeIdx)
    return ppGeneric(pp, node);

  std::string prefix;
  if (sexp.hasTAILCALL())
    prefix += "tail ";
  if (sexp.hasImport())
    prefix += "import ";
  if (sexp.hasV8Intrinsic())
    prefix += "intrinsic ";

  std::string callee = ppOperand(pp, a[calleeIdx]);
  std::string call = callee + "(" + ppList(pp, a, calleeIdx + 1) + ")";
  std::string ctx = calleeIdx ? ppOperand(pp, a[0]) + "->" : "";

  if (eval)
    return prefix + "eval#" + ppNumber(sexp.getJSDirectEval()) + " " + call;
  if (priv)
    return prefix + "private " + ctx + call;
  return prefix + ctx + call;
}

} // namespace IRI_STRUCTURAL
