// Generated Stub for IRI_TAG::JSPropKey (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSPropKey
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Value -> sexp.getArg_Value()
 * Flags: (none)
 */
void computeJSPropKeyVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSPropKey");
  // IRI_GEN::JSPropKeySEXP sexp(node, ptactx.ctx);

  // === TODO : JSPropKey ===
  // if (sexp.hasArg_Value()) { IRID arg_Value = sexp.getArg_Value(); }

  return;
}

} // namespace IRI_STRUCTURAL
