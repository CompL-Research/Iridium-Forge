// Generated Stub for IRI_TAG::JSThrowError
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSThrowError
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSThrowError(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
