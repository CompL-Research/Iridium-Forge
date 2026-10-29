// Generated Stub for IRI_TAG::JSBigInt (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSBigInt
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
std::string printJSBigIntRVal(const PPContext &pp, IRID node) {
  IRI_GEN::JSBigIntSEXP sexp(node, pp.ctx);
  return std::string(pp.str(sexp.getIridiumPrimitive())) + "n";
}

} // namespace IRI_STRUCTURAL
