// Generated Stub for IRI_TAG::ModuleDeclareGuard
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ModuleDeclareGuard
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - double DECL -> sexp.getDECL()
 *   - double EVAL -> sexp.getEVAL()
 */
void handleModuleDeclareGuard(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case ModuleDeclareGuard");
  // IRI_GEN::ModuleDeclareGuardSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : ModuleDeclareGuard ===
  // if (sexp.hasDECL()) { double dbl_DECL = sexp.getDECL(); }
  // if (sexp.hasEVAL()) { double dbl_EVAL = sexp.getEVAL(); }

  return;
}

} // namespace IRI_STRUCTURAL
