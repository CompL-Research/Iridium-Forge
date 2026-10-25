#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include "external/Prakriti.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForInNext
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID IteratorObj -> sexp.getArg_IteratorObj()
 * Flags: (none)
 */
void computeJSForInNextVals(const PTAStatementContext &ptactx, IRID node,
                            std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error(
      "PTA unhandled case JSForInNext: only reachable under a CompoundAssn");
}

} // namespace IRI_STRUCTURAL
