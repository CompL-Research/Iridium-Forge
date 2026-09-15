// Generated Stub for IRI_TAG::CompoundAssn
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: CompoundAssn
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleCompoundAssn(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case CompoundAssn");
  // IRI_GEN::CompoundAssnSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : CompoundAssn ===

  return;
}

} // namespace IRI_STRUCTURAL
