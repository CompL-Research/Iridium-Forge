// Generated Stub for IRI_TAG::ModuleDeclareReturn
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ModuleDeclareReturn
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleModuleDeclareReturn(const PTAStatementContext &ptactx) {
  throw std::runtime_error("PTA unhandled case ModuleDeclareReturn");
  // IRI_GEN::ModuleDeclareReturnSEXP sexp(ptactx.stmt.id, ptactx.ctx);

  // === TODO : ModuleDeclareReturn ===

  return;
}

} // namespace IRI_STRUCTURAL
