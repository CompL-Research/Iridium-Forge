// Generated Stub for IRI_TAG::CompoundAssn
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <iostream>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: CompoundAssn
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleCompoundAssn(const PTAStatementContext &ctx) {
  throw std::runtime_error("PTA unhandled case CompoundAssn");
  // IRI_GEN::CompoundAssnSEXP sexp(ctx.stmt.id, ctx.pool);

  // === TODO : CompoundAssn ===

  return ;
}

} // namespace IRI_STRUCTURAL
