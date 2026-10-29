// Generated Stub for IRI_TAG::UNOPDelMemberExpr
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
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
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printUNOPDelMemberExpr(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
