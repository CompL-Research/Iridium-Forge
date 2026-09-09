// Generated Stub for IRI_TAG::GWrite
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

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
void handleGWrite(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case GWrite");
  // IRI_GEN::GWriteSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : GWrite ===
  // if (sexp.hasArg_LValTarget()) { IRID arg_LValTarget = sexp.getArg_LValTarget(); }
  // if (sexp.hasArg_RVal()) { IRID arg_RVal = sexp.getArg_RVal(); }
  // bool has_INIT = sexp.hasINIT();
  // bool has_SAFE = sexp.hasSAFE();
  // bool has_DECLVAR = sexp.hasDECLVAR();
  // bool has_DECLFUN = sexp.hasDECLFUN();

  return ;
}

} // namespace IRI_STRUCTURAL
