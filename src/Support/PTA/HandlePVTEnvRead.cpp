// Generated Stub for IRI_TAG::PVTEnvRead
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

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
void handlePVTEnvRead(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case PVTEnvRead");
  // IRI_GEN::PVTEnvReadSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : PVTEnvRead ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // bool has_SYMBOL = sexp.hasSYMBOL();
  // bool has_METHOD = sexp.hasMETHOD();
  // bool has_FULLY_RESOLVE = sexp.hasFULLY_RESOLVE();

  return ;
}

} // namespace IRI_STRUCTURAL
