// Generated Stub for IRI_TAG::JSUnop (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSUnop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Val -> sexp.getArg_Val()
 * Flags:
 *   - string OP -> sexp.getOP()
 */
void computeJSUnopVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSUnop");
  // IRI_GEN::JSUnopSEXP sexp(node, ptactx.ctx);

  // === TODO : JSUnop ===
  // if (sexp.hasArg_Val()) { IRID arg_Val = sexp.getArg_Val(); }
  // if (sexp.hasOP()) { StringID str_OP = sexp.getOP(); }

  return;
}

} // namespace IRI_STRUCTURAL
