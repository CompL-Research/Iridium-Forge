// Generated Stub for IRI_TAG::Binop
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Binop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID LBinop -> sexp.getArg_LBinop()
 *   [1] IRID RBinop -> sexp.getArg_RBinop()
 * Flags:
 *   - string OP -> sexp.getOP()
 */
void handleBinop(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case Binop");
  // IRI_GEN::BinopSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : Binop ===
  // if (sexp.hasArg_LBinop()) { IRID arg_LBinop = sexp.getArg_LBinop(); }
  // if (sexp.hasArg_RBinop()) { IRID arg_RBinop = sexp.getArg_RBinop(); }
  // if (sexp.hasOP()) { StringID str_OP = sexp.getOP(); }

  return ;
}

} // namespace IRI_STRUCTURAL
