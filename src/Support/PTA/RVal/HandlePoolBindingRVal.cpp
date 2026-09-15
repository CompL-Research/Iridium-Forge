#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PoolBinding
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Lambda -> sexp.getArg_Lambda()
 * Flags:
 *   - double REFIDX -> sexp.getREFIDX()
 */
void computePoolBindingVals(const PTAStatementContext &ptactx, IRID node,
                           std::set<Prakriti::NodeUID> &res_) {
  IRI_GEN::PoolBindingSEXP sexp(node, ptactx.ctx);
  resolvePKRRVal(ptactx, sexp.getArg_Lambda(), res_);
}

} // namespace IRI_STRUCTURAL
