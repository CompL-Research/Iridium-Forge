// Generated Stub for IRI_TAG::UNOPDelVar (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: UNOPDelVar
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 */
std::string printUNOPDelVarRVal(const PPContext &pp, IRID node) {
  IRI_GEN::UNOPDelVarSEXP sexp(node, pp.ctx);
  // Only the operand of a delete Unop: Unop("delete", x) prints delete x
  return ppName(pp.str(sexp.getNAME()));
}

} // namespace IRI_STRUCTURAL
