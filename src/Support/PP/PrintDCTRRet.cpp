// Generated Stub for IRI_TAG::DCTRRet
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: DCTRRet
 * Meta:    AMP
 * Arguments:
 *   [0] IRID userObj -> sexp.getArg_userObj()
 *   [1] IRID thisObj -> sexp.getArg_thisObj()
 * Flags: (none)
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printDCTRRet(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
