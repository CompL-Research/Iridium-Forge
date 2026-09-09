// Generated Stub for IRI_TAG::UNOPDelVar
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: UNOPDelVar
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 */
void handleUNOPDelVar(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case UNOPDelVar");
  // IRI_GEN::UNOPDelVarSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : UNOPDelVar ===
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }

  return ;
}

} // namespace IRI_STRUCTURAL
