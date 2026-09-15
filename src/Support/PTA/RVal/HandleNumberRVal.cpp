// Generated Stub for IRI_TAG::Number (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Number
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - double IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
void computeNumberVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case Number");
  // IRI_GEN::NumberSEXP sexp(node, ptactx.ctx);

  // === TODO : Number ===
  // if (sexp.hasIridiumPrimitive()) { double dbl_IridiumPrimitive = sexp.getIridiumPrimitive(); }

  return;
}

} // namespace IRI_STRUCTURAL
