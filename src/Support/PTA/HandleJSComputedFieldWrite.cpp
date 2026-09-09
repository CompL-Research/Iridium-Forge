// Generated Stub for IRI_TAG::JSComputedFieldWrite
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSComputedFieldWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 */
void handleJSComputedFieldWrite(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSComputedFieldWrite");
  // IRI_GEN::JSComputedFieldWriteSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSComputedFieldWrite ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }
  // if (sexp.hasArg_Value()) { IRID arg_Value = sexp.getArg_Value(); }
  // bool has_SAFE = sexp.hasSAFE();

  return ;
}

} // namespace IRI_STRUCTURAL
