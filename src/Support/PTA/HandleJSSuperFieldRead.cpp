// Generated Stub for IRI_TAG::JSSuperFieldRead
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSuperFieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID This -> sexp.getArg_This()
 *   [1] IRID Super -> sexp.getArg_Super()
 *   [2] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
void handleJSSuperFieldRead(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case JSSuperFieldRead");
  // IRI_GEN::JSSuperFieldReadSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : JSSuperFieldRead ===
  // if (sexp.hasArg_This()) { IRID arg_This = sexp.getArg_This(); }
  // if (sexp.hasArg_Super()) { IRID arg_Super = sexp.getArg_Super(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }

  return ;
}

} // namespace IRI_STRUCTURAL
