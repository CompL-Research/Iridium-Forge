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
 *   - void   CCall -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_CCall)
 *   - void   ConstructorCall -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_ConstructorCall)
 *   - void   PrivateCall -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_PrivateCall)
 *   - void   Import -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_Import)
 *   - void   Super -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_Super)
 *   - void   V8Intrinsic -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_V8Intrinsic)
 *   - void   TAILCALL -> pp.on(node, IRI_GEN::CallSiteSEXP::FLAG_IDX_TAILCALL)
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
std::string printCallSiteRVal(const PPContext &pp, IRID node) {
  // ctx->f(a, b), same shape as Apply. ctx is `this` for plain/private calls
  // and new.target for super/new (construct). Mirrors the lowering order:
  //   eval#N f(a, b)            [f, args...], no ctx
  //   super nt->f(a, b)         [f, nt, args...], new.target = nt
  //   new f->f(a, b)            [f, args...], new.target = callee
  //   ctx->f(a, b)              CCall: [ctx, f, args...], this = ctx
  //   private ctx->f(a, b)      [ctx, f, args...], brand-checked
  //   f(a, b)                   [f, args...], no ctx
  using S = IRI_GEN::CallSiteSEXP;
  S sexp(node, pp.ctx);
  auto a = pp.args(node);
  bool eval = sexp.hasJSDirectEval();
  bool super = pp.on(node, S::FLAG_IDX_Super);
  bool ctor = pp.on(node, S::FLAG_IDX_ConstructorCall);
  bool ccall = pp.on(node, S::FLAG_IDX_CCall);
  bool priv = pp.on(node, S::FLAG_IDX_PrivateCall);
  size_t calleeIdx = (super || ccall || priv) ? 1 : 0;
  if (a.size() <= calleeIdx)
    return ppGeneric(pp, node);

  std::string prefix;
  if (pp.on(node, S::FLAG_IDX_TAILCALL))
    prefix += "tail ";
  if (pp.on(node, S::FLAG_IDX_Import))
    prefix += "import ";
  if (pp.on(node, S::FLAG_IDX_V8Intrinsic))
    prefix += "intrinsic ";

  std::string callee = ppOperand(pp, a[calleeIdx]);
  std::string call = callee + "(" + ppList(pp, a, calleeIdx + 1) + ")";
  std::string ctx = calleeIdx ? ppOperand(pp, a[0]) + "->" : "";

  if (eval)
    return prefix + "eval#" + ppNumber(sexp.getJSDirectEval()) + " " + call;
  if (super) // the super ctor is pushed first, then new.target
    return prefix + "super " + ppOperand(pp, a[1]) + "->" + ppOperand(pp, a[0]) +
           "(" + ppList(pp, a, 2) + ")";
  if (ctor)
    return prefix + "new " + callee + "->" + call;
  if (priv)
    return prefix + "private " + ctx + call;
  return prefix + ctx + call;
}

} // namespace IRI_STRUCTURAL
