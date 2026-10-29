// Generated Stub for IRI_TAG::JSCatchContext (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCatchContext
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 */
std::string printJSCatchContextRVal(const PPContext &pp, IRID node) {
  IRI_GEN::JSCatchContextSEXP sexp(node, pp.ctx);
  if (sexp.hasNAME())
    return "caught(" + ppName(pp.str(sexp.getNAME())) + ")";
  return "caught";
}

} // namespace IRI_STRUCTURAL
