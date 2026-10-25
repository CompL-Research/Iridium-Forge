#include "Support/PTA/PTAContext.hpp"
#include "external/Prakriti.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSBigInt
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
void computeJSBigIntVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("JSBigIntRVal", node);

  res_.insert(Prakriti::PKRGlobalState::getBIGINT());
}

} // namespace IRI_STRUCTURAL
