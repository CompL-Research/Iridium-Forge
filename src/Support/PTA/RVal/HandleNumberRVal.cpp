#include "Support/PTA/PTAContext.hpp"
#include "external/Prakriti.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Number
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - double IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
void computeNumberVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("NumberRVal", node);

  res_.insert(Prakriti::PKRGlobalState::getNUMBER());
}

} // namespace IRI_STRUCTURAL
