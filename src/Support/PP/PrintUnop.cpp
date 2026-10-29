// Generated Stub for IRI_TAG::Unop
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Unop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Val -> sexp.getArg_Val()
 * Flags:
 *   - string OP -> sexp.getOP()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printUnop(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
