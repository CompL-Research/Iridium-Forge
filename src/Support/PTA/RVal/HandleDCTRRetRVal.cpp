// Generated Stub for IRI_TAG::DCTRRet (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: DCTRRet
 * Meta:    AMP
 * Arguments:
 *   [0] IRID userObj -> sexp.getArg_userObj()
 *   [1] IRID thisObj -> sexp.getArg_thisObj()
 * Flags: (none)
 */
void computeDCTRRetVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case DCTRRet");
  // IRI_GEN::DCTRRetSEXP sexp(node, ptactx.ctx);

  // === TODO : DCTRRet ===
  // if (sexp.hasArg_userObj()) { IRID arg_userObj = sexp.getArg_userObj(); }
  // if (sexp.hasArg_thisObj()) { IRID arg_thisObj = sexp.getArg_thisObj(); }

  return;
}

} // namespace IRI_STRUCTURAL
