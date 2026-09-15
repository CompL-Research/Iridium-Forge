// Generated Stub for IRI_TAG::UNOPDelMemberExpr
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: UNOPDelMemberExpr
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Receiver -> sexp.getArg_Receiver()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handleUNOPDelMemberExpr(const PTAStatementContext &ptactx) {
  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
