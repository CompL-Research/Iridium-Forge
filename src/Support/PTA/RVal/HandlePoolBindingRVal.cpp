// Generated Stub for IRI_TAG::PoolBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

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
  throw std::runtime_error("PTA RVal unhandled case PoolBinding");
  // IRI_GEN::PoolBindingSEXP sexp(node, ptactx.ctx);

  // === TODO : PoolBinding ===
  // if (sexp.hasArg_Lambda()) { IRID arg_Lambda = sexp.getArg_Lambda(); }
  // if (sexp.hasREFIDX()) { double dbl_REFIDX = sexp.getREFIDX(); }

  return;
}

} // namespace IRI_STRUCTURAL
