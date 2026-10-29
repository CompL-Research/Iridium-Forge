// Generated Stub for IRI_TAG::JSCheckBrand
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCheckBrand
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Method -> sexp.getArg_Method()
 * Flags: (none)
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSCheckBrand(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
