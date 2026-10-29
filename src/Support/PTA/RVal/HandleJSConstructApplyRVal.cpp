// Generated Stub for IRI_TAG::JSConstructApply (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSConstructApply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID NewTarget -> sexp.getArg_NewTarget()
 *   [2] IRID ArgList -> sexp.getArg_ArgList()
 * Flags: (none)
 */
void computeJSConstructApplyVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSConstructApply");
  // IRI_GEN::JSConstructApplySEXP sexp(node, ptactx.ctx);

  // === TODO : JSConstructApply ===
  // if (sexp.hasArg_Callee()) { IRID arg_Callee = sexp.getArg_Callee(); }
  // if (sexp.hasArg_NewTarget()) { IRID arg_NewTarget = sexp.getArg_NewTarget(); }
  // if (sexp.hasArg_ArgList()) { IRID arg_ArgList = sexp.getArg_ArgList(); }

  return;
}

} // namespace IRI_STRUCTURAL
