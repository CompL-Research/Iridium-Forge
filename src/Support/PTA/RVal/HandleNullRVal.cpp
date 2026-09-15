// Generated Stub for IRI_TAG::Null (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Null
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - void   IridiumPrimitive -> sexp.hasIridiumPrimitive()
 */
void computeNullVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case Null");
  // IRI_GEN::NullSEXP sexp(node, ptactx.ctx);

  // === TODO : Null ===
  // bool has_IridiumPrimitive = sexp.hasIridiumPrimitive();

  return;
}

} // namespace IRI_STRUCTURAL
