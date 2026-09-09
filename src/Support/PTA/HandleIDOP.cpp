// Generated Stub for IRI_TAG::IDOP
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: IDOP
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   PREFIX -> sexp.getPREFIX()
 *   - bool   INCREMENT -> sexp.getINCREMENT()
 */
void handleIDOP(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case IDOP");
  // IRI_GEN::IDOPSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : IDOP ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasPREFIX()) { bool val_PREFIX = sexp.getPREFIX(); }
  // if (sexp.hasINCREMENT()) { bool val_INCREMENT = sexp.getINCREMENT(); }

  return ;
}

} // namespace IRI_STRUCTURAL
