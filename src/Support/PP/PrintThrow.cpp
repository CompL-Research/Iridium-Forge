// Generated Stub for IRI_TAG::Throw
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Throw
 * Meta:    STMT
 * Arguments:
 *   [0] IRID ThrowVal -> sexp.getArg_ThrowVal()
 * Flags: (none)
 */
std::string printThrow(const PPContext &pp, IRID node) {
  IRI_GEN::ThrowSEXP sexp(node, pp.ctx);
  return "throw " + ppRVal(pp, sexp.getArg_ThrowVal());
}

} // namespace IRI_STRUCTURAL
