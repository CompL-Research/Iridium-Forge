// Generated Stub for IRI_TAG::Apply
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Apply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID Context -> sexp.getArg_Context()
 *   [2] IRID ArgList -> sexp.getArg_ArgList()
 * Flags:
 *   - void   ConstructorCall -> sexp.hasConstructorCall()
 *   - void   Super -> sexp.hasSuper()
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printApply(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
