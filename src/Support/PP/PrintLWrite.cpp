// Generated Stub for IRI_TAG::LWrite
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: LWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID LValTarget -> sexp.getArg_LValTarget()
 *   [1] IRID RVal -> sexp.getArg_RVal()
 * Flags:
 *   - void   INIT -> sexp.hasINIT()
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   THISINIT -> sexp.hasTHISINIT()
 */
std::string printLWrite(const PPContext &pp, IRID node) {
  // x <- v (INIT) / x = v (SAFE) / x .= v (checked)
  // thisinit marks put_loc_check_init; it is ignored when INIT is set.

  IRI_GEN::LWriteSEXP lWrite(node, pp.ctx);
  std::string out = ppWriteLHS(pp, node) + " " + ppRVal(pp, lWrite.getArg_RVal());
  if (lWrite.hasTHISINIT() && !lWrite.hasINIT())
    return "thisinit " + out;
  return out;
}

} // namespace IRI_STRUCTURAL
