#include "Support/PTA/PTAContext.hpp"
#include "external/Prakriti.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Null
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - void   IridiumPrimitive -> sexp.hasIridiumPrimitive()
 */
void computeNullVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("NullRVal", node);

  res_.insert(Prakriti::PKRGlobalState::getNULL());
}

} // namespace IRI_STRUCTURAL
