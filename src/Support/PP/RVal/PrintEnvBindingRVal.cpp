// Generated Stub for IRI_TAG::EnvBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: EnvBinding
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - void   JSARG -> sexp.hasJSARG()
 *   - void   JSRESTARG -> sexp.hasJSRESTARG()
 *   - void   JSLET -> sexp.hasJSLET()
 *   - void   JSCONST -> sexp.hasJSCONST()
 *   - void   JSVAR -> sexp.hasJSVAR()
 *   - string NAME -> sexp.getNAME()
 *   - double REFIDX -> sexp.getREFIDX()
 *   - double SCOPE -> sexp.getSCOPE()
 *   - double NEXT -> sexp.getNEXT()
 *   - double LINK -> sexp.getLINK()
 */
std::string printEnvBindingRVal(const PPContext &pp, IRID node) {
  return ppBinding(pp, node);
}

} // namespace IRI_STRUCTURAL
