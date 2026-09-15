// Generated Stub for IRI_TAG::JSComputedFieldRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSComputedFieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 */
void computeJSComputedFieldReadVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSComputedFieldRead");
  // IRI_GEN::JSComputedFieldReadSEXP sexp(node, ptactx.ctx);

  // === TODO : JSComputedFieldRead ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }
  // bool has_SAFE = sexp.hasSAFE();

  return;
}

} // namespace IRI_STRUCTURAL
