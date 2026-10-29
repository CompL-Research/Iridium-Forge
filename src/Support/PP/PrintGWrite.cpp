// Generated Stub for IRI_TAG::GWrite
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: GWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID LValTarget -> sexp.getArg_LValTarget()
 *   [1] IRID RVal -> sexp.getArg_RVal()
 * Flags:
 *   - void   INIT -> sexp.hasINIT()
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   DECLVAR -> sexp.hasDECLVAR()
 *   - void   DECLFUN -> sexp.hasDECLFUN()
 */
std::string printGWrite(const PPContext &pp, IRID node) {
  IRI_GEN::GWriteSEXP sexp(node, pp.ctx);
  return ppWriteLHS(pp, node) + " " + ppRVal(pp, sexp.getArg_RVal());
}

} // namespace IRI_STRUCTURAL
