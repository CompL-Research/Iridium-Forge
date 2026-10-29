// Generated Stub for IRI_TAG::UNOPDelVar
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: UNOPDelVar
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printUNOPDelVar(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
