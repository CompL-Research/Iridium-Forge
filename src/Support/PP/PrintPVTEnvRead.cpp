// Generated Stub for IRI_TAG::PVTEnvRead
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PVTEnvRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - void   SYMBOL -> sexp.hasSYMBOL()
 *   - void   METHOD -> sexp.hasMETHOD()
 *   - void   FULLY_RESOLVE -> sexp.hasFULLY_RESOLVE()
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printPVTEnvRead(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
