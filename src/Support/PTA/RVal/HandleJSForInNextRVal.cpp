// Generated Stub for IRI_TAG::JSForInNext (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForInNext
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID IteratorObj -> sexp.getArg_IteratorObj()
 * Flags: (none)
 */
void computeJSForInNextVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSForInNext");
  // IRI_GEN::JSForInNextSEXP sexp(node, ptactx.ctx);

  // === TODO : JSForInNext ===
  // if (sexp.hasArg_IteratorObj()) { IRID arg_IteratorObj = sexp.getArg_IteratorObj(); }

  return;
}

} // namespace IRI_STRUCTURAL
