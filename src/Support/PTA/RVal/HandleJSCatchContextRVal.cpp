// Generated Stub for IRI_TAG::JSCatchContext (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCatchContext
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 */
void computeJSCatchContextVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSCatchContext");
  // IRI_GEN::JSCatchContextSEXP sexp(node, ptactx.ctx);

  // === TODO : JSCatchContext ===
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }

  return;
}

} // namespace IRI_STRUCTURAL
