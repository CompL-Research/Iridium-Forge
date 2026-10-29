// Generated Stub for IRI_TAG::JSThrowError (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSThrowError
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 */
void computeJSThrowErrorVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSThrowError");
  // IRI_GEN::JSThrowErrorSEXP sexp(node, ptactx.ctx);

  // === TODO : JSThrowError ===
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }

  return;
}

} // namespace IRI_STRUCTURAL
