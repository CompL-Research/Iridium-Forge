// Generated Stub for IRI_TAG::EnvRead
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: EnvRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   TAINTED -> sexp.hasTAINTED()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printEnvRead(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
