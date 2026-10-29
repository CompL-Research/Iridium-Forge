// Generated Stub for IRI_TAG::JSEval (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSEval
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 * Flags:
 *   - double ScopeIDX -> sexp.getScopeIDX()
 */
void computeJSEvalVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSEval");
  // IRI_GEN::JSEvalSEXP sexp(node, ptactx.ctx);

  // === TODO : JSEval ===
  // if (sexp.hasArg_Callee()) { IRID arg_Callee = sexp.getArg_Callee(); }
  // if (sexp.hasScopeIDX()) { double dbl_ScopeIDX = sexp.getScopeIDX(); }

  return;
}

} // namespace IRI_STRUCTURAL
