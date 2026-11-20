// Generated Stub for IRI_TAG::ModuleDecl (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ModuleDecl
 * Meta:    RVAL
 * Arguments: (none)
 * Flags: (none)
 */
void computeModuleDeclVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case ModuleDecl");
  // IRI_GEN::ModuleDeclSEXP sexp(node, ptactx.ctx);

  // === TODO : ModuleDecl ===

  return;
}

} // namespace IRI_STRUCTURAL
