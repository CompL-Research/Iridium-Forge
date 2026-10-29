// Generated Stub for IRI_TAG::JSEval
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSEval
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 * Flags:
 *   - double ScopeIDX -> sexp.getScopeIDX()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSEval(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
