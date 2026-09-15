// Generated Stub for IRI_TAG::ThisINIT (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ThisINIT
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID oldVal -> sexp.getArg_oldVal()
 *   [1] IRID newVal -> sexp.getArg_newVal()
 * Flags: (none)
 */
void computeThisINITVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case ThisINIT");
  // IRI_GEN::ThisINITSEXP sexp(node, ptactx.ctx);

  // === TODO : ThisINIT ===
  // if (sexp.hasArg_oldVal()) { IRID arg_oldVal = sexp.getArg_oldVal(); }
  // if (sexp.hasArg_newVal()) { IRID arg_newVal = sexp.getArg_newVal(); }

  return;
}

} // namespace IRI_STRUCTURAL
