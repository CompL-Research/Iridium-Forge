#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "external/Prakriti.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Boolean
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - bool   IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
void computeBooleanVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("BooleanRVal", node);

  IRI_GEN::BooleanSEXP sexp(node, ptactx.ctx);
  res_.insert(sexp.getIridiumPrimitive() ? Prakriti::PKRGlobalState::getTRUE()
                                         : Prakriti::PKRGlobalState::getFALSE());
}

} // namespace IRI_STRUCTURAL
