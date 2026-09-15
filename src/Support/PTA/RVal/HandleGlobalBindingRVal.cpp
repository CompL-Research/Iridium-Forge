// Generated Stub for IRI_TAG::GlobalBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: GlobalBinding
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 *   - double LINK -> sexp.getLINK()
 */
void computeGlobalBindingVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case GlobalBinding");
  // IRI_GEN::GlobalBindingSEXP sexp(node, ptactx.ctx);

  // === TODO : GlobalBinding ===
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }
  // if (sexp.hasLINK()) { double dbl_LINK = sexp.getLINK(); }

  return;
}

} // namespace IRI_STRUCTURAL
