// Generated Stub for IRI_TAG::Ret
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Ret
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleRet(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case Ret");
  // IRI_GEN::RetSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : Ret ===

  return;
}

} // namespace IRI_STRUCTURAL
