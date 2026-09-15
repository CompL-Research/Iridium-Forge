// Generated Stub for IRI_TAG::JSForInStart (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForInStart
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void computeJSForInStartVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSForInStart");
  // IRI_GEN::JSForInStartSEXP sexp(node, ptactx.ctx);

  // === TODO : JSForInStart ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }

  return;
}

} // namespace IRI_STRUCTURAL
