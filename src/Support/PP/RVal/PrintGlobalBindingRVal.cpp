// Generated Stub for IRI_TAG::GlobalBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: GlobalBinding
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 *   - double LINK -> sexp.getLINK()
 */
std::string printGlobalBindingRVal(const PPContext &pp, IRID node) {
  return ppBinding(pp, node);
}

} // namespace IRI_STRUCTURAL
