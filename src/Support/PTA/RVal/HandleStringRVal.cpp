// Generated Stub for IRI_TAG::String (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: String
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
void computeStringVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case String");
  // IRI_GEN::StringSEXP sexp(node, ptactx.ctx);

  // === TODO : String ===
  // if (sexp.hasIridiumPrimitive()) { StringID str_IridiumPrimitive = sexp.getIridiumPrimitive(); }

  return;
}

} // namespace IRI_STRUCTURAL
