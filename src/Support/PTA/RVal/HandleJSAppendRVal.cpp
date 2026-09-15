// Generated Stub for IRI_TAG::JSAppend (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSAppend
 * Meta:    AMP
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID InsertionIdx -> sexp.getArg_InsertionIdx()
 *   [2] IRID SpreadObj -> sexp.getArg_SpreadObj()
 * Flags: (none)
 */
void computeJSAppendVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSAppend");
  // IRI_GEN::JSAppendSEXP sexp(node, ptactx.ctx);

  // === TODO : JSAppend ===
  // if (sexp.hasArg_TargetObj()) { IRID arg_TargetObj = sexp.getArg_TargetObj(); }
  // if (sexp.hasArg_InsertionIdx()) { IRID arg_InsertionIdx = sexp.getArg_InsertionIdx(); }
  // if (sexp.hasArg_SpreadObj()) { IRID arg_SpreadObj = sexp.getArg_SpreadObj(); }

  return;
}

} // namespace IRI_STRUCTURAL
