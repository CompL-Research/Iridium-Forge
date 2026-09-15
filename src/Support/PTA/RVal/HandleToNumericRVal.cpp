// Generated Stub for IRI_TAG::ToNumeric (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ToNumeric
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void computeToNumericVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case ToNumeric");
  // IRI_GEN::ToNumericSEXP sexp(node, ptactx.ctx);

  // === TODO : ToNumeric ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }

  return;
}

} // namespace IRI_STRUCTURAL
