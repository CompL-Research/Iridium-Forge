// Generated Stub for IRI_TAG::UNOPDelMemberExpr (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: UNOPDelMemberExpr
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Receiver -> sexp.getArg_Receiver()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
void computeUNOPDelMemberExprVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case UNOPDelMemberExpr");
  // IRI_GEN::UNOPDelMemberExprSEXP sexp(node, ptactx.ctx);

  // === TODO : UNOPDelMemberExpr ===
  // if (sexp.hasArg_Receiver()) { IRID arg_Receiver = sexp.getArg_Receiver(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }

  return;
}

} // namespace IRI_STRUCTURAL
