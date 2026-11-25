// Generated Stub for IRI_TAG::LambdaRef (RVAL)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: LambdaRef
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Lambda -> sexp.getArg_Lambda()
 * Flags:
 *   - bool   CNAME -> sexp.getCNAME()
 *   - bool   SETNAME -> sexp.getSETNAME()
 *   - string NAME -> sexp.getNAME()
 */
std::string printLambdaRefRVal(const PPContext &pp, IRID node) {
  // λ f@c2 (a site materializing closure 2 under the name f), λ@c2 unnamed
  IRI_GEN::LambdaRefSEXP sexp(node, pp.ctx);
  std::string name(sexp.hasNAME() ? pp.str(sexp.getNAME()) : "");
  return "λ" + (name.empty() ? "" : " " + name) + "@" +
         ppOperand(pp, sexp.getArg_Lambda());
}

} // namespace IRI_STRUCTURAL
