// Generated Stub for IRI_TAG::CallSite (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: CallSite
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - void   CCall -> sexp.hasCCall()
 *   - void   ConstructorCall -> sexp.hasConstructorCall()
 *   - void   PrivateCall -> sexp.hasPrivateCall()
 *   - void   Import -> sexp.hasImport()
 *   - void   Super -> sexp.hasSuper()
 *   - void   V8Intrinsic -> sexp.hasV8Intrinsic()
 *   - void   TAILCALL -> sexp.hasTAILCALL()
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
void computeCallSiteVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case CallSite");
  // IRI_GEN::CallSiteSEXP sexp(node, ptactx.ctx);

  // === TODO : CallSite ===
  // bool has_CCall = sexp.hasCCall();
  // bool has_ConstructorCall = sexp.hasConstructorCall();
  // bool has_PrivateCall = sexp.hasPrivateCall();
  // bool has_Import = sexp.hasImport();
  // bool has_Super = sexp.hasSuper();
  // bool has_V8Intrinsic = sexp.hasV8Intrinsic();
  // bool has_TAILCALL = sexp.hasTAILCALL();
  // if (sexp.hasJSDirectEval()) { double dbl_JSDirectEval = sexp.getJSDirectEval(); }

  return;
}

} // namespace IRI_STRUCTURAL
