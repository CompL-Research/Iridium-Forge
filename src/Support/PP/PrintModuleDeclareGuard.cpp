// Generated Stub for IRI_TAG::ModuleDeclareGuard
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ModuleDeclareGuard
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - double DECL -> sexp.getDECL()
 *   - double EVAL -> sexp.getEVAL()
 */
std::string printModuleDeclareGuard(const PPContext &pp, IRID node) {
  // IRI_GEN::ModuleDeclareGuardSEXP sexp(node, pp.ctx);

  // === TODO : ModuleDeclareGuard ===
  // if (sexp.hasDECL()) { double dbl_DECL = sexp.getDECL(); }
  // if (sexp.hasEVAL()) { double dbl_EVAL = sexp.getEVAL(); }

  return ppGeneric(pp, node);
}

} // namespace IRI_STRUCTURAL
