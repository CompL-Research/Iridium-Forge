// Generated Stub for IRI_TAG::Number (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Number
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - double IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
std::string printNumberRVal(const PPContext &pp, IRID node) {
  IRI_GEN::NumberSEXP sexp(node, pp.ctx);
  return ppNumber(sexp.getIridiumPrimitive());
}

} // namespace IRI_STRUCTURAL
