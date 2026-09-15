// Generated Stub for IRI_TAG::JSArray (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSArray
 * Meta:    RVAL
 * Arguments: (none)
 * Flags: (none)
 */
void computeJSArrayVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSArray");
  // IRI_GEN::JSArraySEXP sexp(node, ptactx.ctx);

  // === TODO : JSArray ===

  return;
}

} // namespace IRI_STRUCTURAL
