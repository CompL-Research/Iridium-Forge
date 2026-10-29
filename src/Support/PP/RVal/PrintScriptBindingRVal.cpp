// Generated Stub for IRI_TAG::ScriptBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ScriptBinding
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - void   JSLET -> sexp.hasJSLET()
 *   - void   JSCONST -> sexp.hasJSCONST()
 *   - void   JSVAR -> sexp.hasJSVAR()
 *   - string NAME -> sexp.getNAME()
 *   - double LINK -> sexp.getLINK()
 */
std::string printScriptBindingRVal(const PPContext &pp, IRID node) {
  return ppBinding(pp, node);
}

} // namespace IRI_STRUCTURAL
