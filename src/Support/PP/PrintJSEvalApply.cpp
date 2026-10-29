// Generated Stub for IRI_TAG::JSEvalApply
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSEvalApply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID ArgList -> sexp.getArg_ArgList()
 * Flags:
 *   - double ScopeIDX -> sexp.getScopeIDX()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSEvalApply(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
