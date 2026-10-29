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
 * Flags:
 *   - void   Super -> sexp.hasSuper()
 */
std::string printJSConstructRVal(const PPContext &pp, IRID node) {
  // new nt->F(a, b): [[Construct]] of F with new.target nt (style A: the
  // context before -> is new.target). new[super] marks a super() call.
  IRI_GEN::JSConstructSEXP sexp(node, pp.ctx);
  auto a = pp.args(node);
  std::string args = "(" + ppList(pp, a, 2) + ")";
  return std::string(sexp.hasSuper() ? "new[super] " : "new ") +
         ppOperand(pp, sexp.getArg_NewTarget()) + "->" +
         ppOperand(pp, sexp.getArg_Callee()) + args;
}

} // namespace IRI_STRUCTURAL
