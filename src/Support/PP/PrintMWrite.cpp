// Generated Stub for IRI_TAG::MWrite
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: MWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID LValTarget -> sexp.getArg_LValTarget()
 *   [1] IRID RVal -> sexp.getArg_RVal()
 * Flags:
 *   - void   INIT -> sexp.hasINIT()
 *   - void   SAFE -> sexp.hasSAFE()
 */
std::string printMWrite(const PPContext &pp, IRID node) {
  IRI_GEN::MWriteSEXP sexp(node, pp.ctx);
  return ppWriteLHS(pp, node) + " " + ppRVal(pp, sexp.getArg_RVal());
}

} // namespace IRI_STRUCTURAL
