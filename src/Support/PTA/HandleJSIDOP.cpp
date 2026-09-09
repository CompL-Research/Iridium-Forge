// Generated Stub for IRI_TAG::JSIDOP
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSIDOP
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   PREFIX -> sexp.getPREFIX()
 *   - bool   INCREMENT -> sexp.getINCREMENT()
 */
void handleJSIDOP(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSIDOP");
  // IRI_GEN::JSIDOPSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSIDOP ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasPREFIX()) { bool val_PREFIX = sexp.getPREFIX(); }
  // if (sexp.hasINCREMENT()) { bool val_INCREMENT = sexp.getINCREMENT(); }

  return ;
}

} // namespace IRI_STRUCTURAL
