// Generated Stub for IRI_TAG::JSForInStart (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForInStart
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printJSForInStartRVal(const PPContext &pp, IRID node) {
  IRI_GEN::JSForInStartSEXP sexp(node, pp.ctx);
  return "forin.start(" + ppRVal(pp, sexp.getArg_Obj()) + ")";
}

} // namespace IRI_STRUCTURAL
