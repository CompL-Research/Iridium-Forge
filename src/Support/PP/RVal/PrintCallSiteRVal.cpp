// Generated Stub for IRI_TAG::CallSite (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: CallSite
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID This -> sexp.getArg_This()
 * Flags: (none)
 */
std::string printCallSiteRVal(const PPContext &pp, IRID node) {
  // this->f(a, b): [callee, this, args...]. A plain call passes g:undefined.
  IRI_GEN::CallSiteSEXP sexp(node, pp.ctx);
  auto a = pp.args(node);
  return ppOperand(pp, sexp.getArg_This()) + "->" +
         ppOperand(pp, sexp.getArg_Callee()) + "(" + ppList(pp, a, 2) + ")";
}

} // namespace IRI_STRUCTURAL
