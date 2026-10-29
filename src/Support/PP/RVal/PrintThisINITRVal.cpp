// Generated Stub for IRI_TAG::ThisINIT (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ThisINIT
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID oldVal -> sexp.getArg_oldVal()
 *   [1] IRID newVal -> sexp.getArg_newVal()
 * Flags: (none)
 */
std::string printThisINITRVal(const PPContext &pp, IRID node) {
  IRI_GEN::ThisINITSEXP sexp(node, pp.ctx);
  return "this.init(" + ppRVal(pp, sexp.getArg_oldVal()) + ", " +
         ppRVal(pp, sexp.getArg_newVal()) + ")";
}

} // namespace IRI_STRUCTURAL
