// Generated Stub for IRI_TAG::Lambda (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Lambda
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - bool   CNAME -> sexp.getCNAME()
 *   - bool   SETNAME -> sexp.getSETNAME()
 *   - string NAME -> sexp.getNAME()
 *   - double StartBBIDX -> sexp.getStartBBIDX()
 */
std::string printLambdaRVal(const PPContext &pp, IRID node) {
  // λ f@BB1 (a closure whose body starts at BB1), λ@BB1 without a name
  IRI_GEN::LambdaSEXP sexp(node, pp.ctx);
  std::string name(sexp.hasNAME() ? pp.str(sexp.getNAME()) : "");
  return "λ" + (name.empty() ? "" : " " + name) + "@" +
         ppBB(sexp.getStartBBIDX());
}

} // namespace IRI_STRUCTURAL
