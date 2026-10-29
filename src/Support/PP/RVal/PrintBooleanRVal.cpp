// Generated Stub for IRI_TAG::Boolean (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Boolean
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - bool   IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
std::string printBooleanRVal(const PPContext &pp, IRID node) {
  IRI_GEN::BooleanSEXP sexp(node, pp.ctx);
  return sexp.hasIridiumPrimitive() && sexp.getIridiumPrimitive() ? "true"
                                                                   : "false";
}

} // namespace IRI_STRUCTURAL
