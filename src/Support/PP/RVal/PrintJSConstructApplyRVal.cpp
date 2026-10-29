// Generated Stub for IRI_TAG::JSConstructApply (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSConstructApply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID NewTarget -> sexp.getArg_NewTarget()
 *   [2] IRID ArgList -> sexp.getArg_ArgList()
 * Flags: (none)
 */
std::string printJSConstructApplyRVal(const PPContext &pp, IRID node) {
  // new nt->F(a, b): [[Construct]] of F with new.target nt (style A: the
  // context before -> is new.target)
  IRI_GEN::JSConstructApplySEXP sexp(node, pp.ctx);
  std::string args = "(..." + ppOperand(pp, sexp.getArg_ArgList()) + ")";
  return "new " +
         ppOperand(pp, sexp.getArg_NewTarget()) + "->" +
         ppOperand(pp, sexp.getArg_Callee()) + args;
}

} // namespace IRI_STRUCTURAL
