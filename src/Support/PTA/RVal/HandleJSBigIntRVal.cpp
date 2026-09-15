// Generated Stub for IRI_TAG::JSBigInt (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSBigInt
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
void computeJSBigIntVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSBigInt");
  // IRI_GEN::JSBigIntSEXP sexp(node, ptactx.ctx);

  // === TODO : JSBigInt ===
  // if (sexp.hasIridiumPrimitive()) { StringID str_IridiumPrimitive = sexp.getIridiumPrimitive(); }

  return;
}

} // namespace IRI_STRUCTURAL
