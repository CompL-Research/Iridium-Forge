// Generated Stub for IRI_TAG::PVTEnvRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
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
 */
std::string printPVTEnvReadRVal(const PPContext &pp, IRID node) {
  // Resolve the private name bound in obj: #(obj)
  IRI_GEN::PVTEnvReadSEXP sexp(node, pp.ctx);
  std::string out = "#(" + ppRVal(pp, sexp.getArg_Obj()) + ")";
  if (sexp.hasMETHOD())
    out += ".method";
  return out;
}

} // namespace IRI_STRUCTURAL
