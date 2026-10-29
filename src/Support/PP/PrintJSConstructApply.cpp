// Generated Stub for IRI_TAG::JSConstructApply
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSConstructApply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID NewTarget -> sexp.getArg_NewTarget()
 *   [2] IRID ArgList -> sexp.getArg_ArgList()
 * Flags:
 *   - void   Super -> sexp.hasSuper()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSConstructApply(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
