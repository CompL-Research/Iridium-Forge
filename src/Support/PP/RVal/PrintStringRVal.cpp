// Generated Stub for IRI_TAG::String (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: String
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
std::string printStringRVal(const PPContext &pp, IRID node) {
  IRI_GEN::StringSEXP sexp(node, pp.ctx);
  return ppStringLit(pp.str(sexp.getIridiumPrimitive()));
}

} // namespace IRI_STRUCTURAL
