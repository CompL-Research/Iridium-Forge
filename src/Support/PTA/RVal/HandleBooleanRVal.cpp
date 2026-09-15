// Generated Stub for IRI_TAG::Boolean (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Boolean
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - bool   IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
void computeBooleanVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case Boolean");
  // IRI_GEN::BooleanSEXP sexp(node, ptactx.ctx);

  // === TODO : Boolean ===
  // if (sexp.hasIridiumPrimitive()) { bool val_IridiumPrimitive = sexp.getIridiumPrimitive(); }

  return;
}

} // namespace IRI_STRUCTURAL
