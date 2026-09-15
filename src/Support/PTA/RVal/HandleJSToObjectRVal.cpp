// Generated Stub for IRI_TAG::JSToObject (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSToObject
 * Meta:    AMP
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 * Flags: (none)
 */
void computeJSToObjectVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSToObject");
  // IRI_GEN::JSToObjectSEXP sexp(node, ptactx.ctx);

  // === TODO : JSToObject ===
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }

  return;
}

} // namespace IRI_STRUCTURAL
