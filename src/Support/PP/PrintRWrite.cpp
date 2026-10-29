// Generated Stub for IRI_TAG::RWrite
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: RWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID LValTarget -> sexp.getArg_LValTarget()
 *   [1] IRID RVal -> sexp.getArg_RVal()
 * Flags:
 *   - void   INIT -> sexp.hasINIT()
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   THISINIT -> sexp.hasTHISINIT()
 */
std::string printRWrite(const PPContext &pp, IRID node) {
  // ^x <- v (INIT) / ^x = v (SAFE) / ^x .= v (checked) / thisinit ^x .= v
  IRI_GEN::RWriteSEXP sexp(node, pp.ctx);
  return ppWriteLHS(pp, node) + " " + ppRVal(pp, sexp.getArg_RVal());
}

} // namespace IRI_STRUCTURAL
