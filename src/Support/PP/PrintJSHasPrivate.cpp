// Generated Stub for IRI_TAG::JSHasPrivate
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSHasPrivate
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Name -> sexp.getArg_Name()
 * Flags: (none)
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSHasPrivate(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
