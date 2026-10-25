// Generated Stub for IRI_TAG::UNOPDelVar (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: UNOPDelVar
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 */
void computeUNOPDelVarVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case UNOPDelVar");
  // IRI_GEN::UNOPDelVarSEXP sexp(node, ptactx.ctx);

  // === TODO : UNOPDelVar ===
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }

  return;
}

} // namespace IRI_STRUCTURAL
