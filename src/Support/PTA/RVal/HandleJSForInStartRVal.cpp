#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include "external/Prakriti.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForInStart
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void computeJSForInStartVals(const PTAStatementContext &ptactx, IRID node,
                             std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("JSForInStartRVal", node);

  IRI_GEN::JSForInStartSEXP sexp(node, ptactx.ctx);
  resolvePKRRVal(ptactx, sexp.getArg_Obj(), res_);
}

} // namespace IRI_STRUCTURAL
