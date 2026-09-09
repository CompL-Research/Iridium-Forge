// Generated Stub for IRI_TAG::FieldRead
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: FieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
void handleFieldRead(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case FieldRead");
  // IRI_GEN::FieldReadSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : FieldRead ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }

  return ;
}

} // namespace IRI_STRUCTURAL
