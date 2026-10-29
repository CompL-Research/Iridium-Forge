// Generated Stub for IRI_TAG::JSToObject (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSToObject
 * Meta:    AMP
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 * Flags: (none)
 */
std::string printJSToObjectRVal(const PPContext &pp, IRID node) {
  IRI_GEN::JSToObjectSEXP sexp(node, pp.ctx);
  return "toobject(" + ppRVal(pp, sexp.getArg_TargetObj()) + ")";
}

} // namespace IRI_STRUCTURAL
