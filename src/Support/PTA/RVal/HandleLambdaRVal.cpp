// Generated Stub for IRI_TAG::Lambda (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Lambda
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - bool   CNAME -> sexp.getCNAME()
 *   - bool   SETNAME -> sexp.getSETNAME()
 *   - string NAME -> sexp.getNAME()
 *   - double StartBBIDX -> sexp.getStartBBIDX()
 */
void computeLambdaVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case Lambda");
  // IRI_GEN::LambdaSEXP sexp(node, ptactx.ctx);

  // === TODO : Lambda ===
  // if (sexp.hasCNAME()) { bool val_CNAME = sexp.getCNAME(); }
  // if (sexp.hasSETNAME()) { bool val_SETNAME = sexp.getSETNAME(); }
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }
  // if (sexp.hasStartBBIDX()) { double dbl_StartBBIDX = sexp.getStartBBIDX(); }

  return;
}

} // namespace IRI_STRUCTURAL
