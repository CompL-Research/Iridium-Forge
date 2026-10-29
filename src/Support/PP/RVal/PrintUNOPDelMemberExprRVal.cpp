// Generated Stub for IRI_TAG::UNOPDelMemberExpr (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: UNOPDelMemberExpr
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Receiver -> sexp.getArg_Receiver()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
std::string printUNOPDelMemberExprRVal(const PPContext &pp, IRID node) {
  IRI_GEN::UNOPDelMemberExprSEXP sexp(node, pp.ctx);
  // Only the operand of a delete Unop: Unop("delete", o.x) prints delete o.x
  return ppMember(pp, sexp.getArg_Receiver(), sexp.getArg_Field());
}

} // namespace IRI_STRUCTURAL
