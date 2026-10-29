// Generated Stub for IRI_TAG::JSIDOP
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSIDOP
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   PREFIX -> sexp.getPREFIX()
 *   - bool   INCREMENT -> sexp.getINCREMENT()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSIDOP(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
