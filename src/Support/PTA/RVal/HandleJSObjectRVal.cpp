// Generated Stub for IRI_TAG::JSObject (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSObject
 * Meta:    RVAL
 * Arguments: (none)
 * Flags: (none)
 */
void computeJSObjectVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSObject");
  // IRI_GEN::JSObjectSEXP sexp(node, ptactx.ctx);

  // === TODO : JSObject ===

  return;
}

} // namespace IRI_STRUCTURAL
