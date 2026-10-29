// Generated Stub for IRI_TAG::JSThrowError (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSThrowError
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 */
std::string printJSThrowErrorRVal(const PPContext &pp, IRID node) {
  // throw.readonly "#x": throws TypeError "'#x' is read-only"
  IRI_GEN::JSThrowErrorSEXP sexp(node, pp.ctx);
  return "throw.readonly " + ppStringLit(pp.str(sexp.getNAME()));
}

} // namespace IRI_STRUCTURAL
