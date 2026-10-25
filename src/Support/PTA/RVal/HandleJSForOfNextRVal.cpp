// Generated Stub for IRI_TAG::JSForOfNext (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForOfNext
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   AWAIT -> sexp.getAWAIT()
 */
void computeJSForOfNextVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSForOfNext");
  // IRI_GEN::JSForOfNextSEXP sexp(node, ptactx.ctx);

  // === TODO : JSForOfNext ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasAWAIT()) { bool val_AWAIT = sexp.getAWAIT(); }

  return;
}

} // namespace IRI_STRUCTURAL
