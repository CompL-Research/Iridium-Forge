// Generated Stub for IRI_TAG::JSEval (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSEval
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 * Flags:
 *   - double ScopeIDX -> sexp.getScopeIDX()
 */
std::string printJSEvalRVal(const PPContext &pp, IRID node) {
  // eval#N f(a): direct eval, N is the scope it can see (ScopeIDX); no `this`
  IRI_GEN::JSEvalSEXP sexp(node, pp.ctx);
  auto a = pp.args(node);
  std::string args = "(" + ppList(pp, a, 1) + ")";
  return "eval#" + ppNumber(sexp.getScopeIDX()) + " " +
         ppOperand(pp, sexp.getArg_Callee()) + args;
}

} // namespace IRI_STRUCTURAL
