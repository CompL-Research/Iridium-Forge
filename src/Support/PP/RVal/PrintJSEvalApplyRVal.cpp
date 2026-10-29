// Generated Stub for IRI_TAG::JSEvalApply (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSEvalApply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID ArgList -> sexp.getArg_ArgList()
 * Flags:
 *   - double ScopeIDX -> sexp.getScopeIDX()
 */
std::string printJSEvalApplyRVal(const PPContext &pp, IRID node) {
  // eval#N f(a): direct eval, N is the scope it can see (ScopeIDX); no `this`
  IRI_GEN::JSEvalApplySEXP sexp(node, pp.ctx);
  std::string args = "(..." + ppOperand(pp, sexp.getArg_ArgList()) + ")";
  return "eval#" + ppNumber(sexp.getScopeIDX()) + " " +
         ppOperand(pp, sexp.getArg_Callee()) + args;
}

} // namespace IRI_STRUCTURAL
