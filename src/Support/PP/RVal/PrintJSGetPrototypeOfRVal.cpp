// Generated Stub for IRI_TAG::JSGetPrototypeOf (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSGetPrototypeOf
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printJSGetPrototypeOfRVal(const PPContext &pp, IRID node) {
  IRI_GEN::JSGetPrototypeOfSEXP sexp(node, pp.ctx);
  return "getprototypeof(" + ppRVal(pp, sexp.getArg_Obj()) + ")";
}

} // namespace IRI_STRUCTURAL
