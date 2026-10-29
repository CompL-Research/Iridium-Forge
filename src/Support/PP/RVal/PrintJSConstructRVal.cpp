// Generated Stub for IRI_TAG::JSConstruct (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSConstruct
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID NewTarget -> sexp.getArg_NewTarget()
 * Flags: (none)
 */
std::string printJSConstructRVal(const PPContext &pp, IRID node) {
  // new nt->F(a, b): [[Construct]] of F with new.target nt (style A: the
  // context before -> is new.target)
  IRI_GEN::JSConstructSEXP sexp(node, pp.ctx);
  auto a = pp.args(node);
  std::string args = "(" + ppList(pp, a, 2) + ")";
  return "new " +
         ppOperand(pp, sexp.getArg_NewTarget()) + "->" +
         ppOperand(pp, sexp.getArg_Callee()) + args;
}

} // namespace IRI_STRUCTURAL
