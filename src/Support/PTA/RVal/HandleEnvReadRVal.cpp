// Generated Stub for IRI_TAG::EnvRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: EnvRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   TAINTED -> sexp.hasTAINTED()
 */
void computeEnvReadVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case EnvRead");
  // IRI_GEN::EnvReadSEXP sexp(node, ptactx.ctx);

  // === TODO : EnvRead ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // bool has_SAFE = sexp.hasSAFE();
  // bool has_TAINTED = sexp.hasTAINTED();

  return;
}

} // namespace IRI_STRUCTURAL
