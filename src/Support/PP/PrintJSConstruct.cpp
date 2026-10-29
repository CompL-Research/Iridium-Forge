// Generated Stub for IRI_TAG::JSConstruct
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSConstruct
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID NewTarget -> sexp.getArg_NewTarget()
 * Flags: (none)
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSConstruct(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
