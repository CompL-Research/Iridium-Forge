// Generated Stub for IRI_TAG::LambdaRef (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: LambdaRef
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Lambda -> sexp.getArg_Lambda()
 * Flags:
 *   - bool   CNAME -> sexp.getCNAME()
 *   - bool   SETNAME -> sexp.getSETNAME()
 *   - string NAME -> sexp.getNAME()
 */
void computeLambdaRefVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case LambdaRef");
  // IRI_GEN::LambdaRefSEXP sexp(node, ptactx.ctx);

  // === TODO : LambdaRef ===
  // if (sexp.hasArg_Lambda()) { IRID arg_Lambda = sexp.getArg_Lambda(); }
  // if (sexp.hasCNAME()) { bool val_CNAME = sexp.getCNAME(); }
  // if (sexp.hasSETNAME()) { bool val_SETNAME = sexp.getSETNAME(); }
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }

  return;
}

} // namespace IRI_STRUCTURAL
