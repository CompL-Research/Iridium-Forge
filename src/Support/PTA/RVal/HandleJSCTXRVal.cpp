// Generated Stub for IRI_TAG::JSCTX (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCTX
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - double OPID -> sexp.getOPID()
 */
void computeJSCTXVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSCTX");
  // IRI_GEN::JSCTXSEXP sexp(node, ptactx.ctx);

  // === TODO : JSCTX ===
  // if (sexp.hasOPID()) { double dbl_OPID = sexp.getOPID(); }

  return;
}

} // namespace IRI_STRUCTURAL
