// Generated Stub for IRI_TAG::CallSite
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: CallSite
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - void   CCall -> sexp.hasCCall()
 *   - void   ConstructorCall -> sexp.hasConstructorCall()
 *   - void   PrivateCall -> sexp.hasPrivateCall()
 *   - void   Import -> sexp.hasImport()
 *   - void   Super -> sexp.hasSuper()
 *   - void   V8Intrinsic -> sexp.hasV8Intrinsic()
 *   - void   TAILCALL -> sexp.hasTAILCALL()
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printCallSite(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
