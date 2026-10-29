// Generated Stub for IRI_TAG::JSEvalApply (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSEvalApply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID ArgList -> sexp.getArg_ArgList()
 * Flags:
 *   - double ScopeIDX -> sexp.getScopeIDX()
 */
void computeJSEvalApplyVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSEvalApply");
  // IRI_GEN::JSEvalApplySEXP sexp(node, ptactx.ctx);

  // === TODO : JSEvalApply ===
  // if (sexp.hasArg_Callee()) { IRID arg_Callee = sexp.getArg_Callee(); }
  // if (sexp.hasArg_ArgList()) { IRID arg_ArgList = sexp.getArg_ArgList(); }
  // if (sexp.hasScopeIDX()) { double dbl_ScopeIDX = sexp.getScopeIDX(); }

  return;
}

} // namespace IRI_STRUCTURAL
