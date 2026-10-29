// Generated Stub for IRI_TAG::JSUnop
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSUnop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Val -> sexp.getArg_Val()
 * Flags:
 *   - string OP -> sexp.getOP()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSUnop(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
