// Generated Stub for IRI_TAG::Unop (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Unop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Val -> sexp.getArg_Val()
 * Flags:
 *   - string OP -> sexp.getOP()
 */
void computeUnopVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case Unop");
  // IRI_GEN::UnopSEXP sexp(node, ptactx.ctx);

  // === TODO : Unop ===
  // if (sexp.hasArg_Val()) { IRID arg_Val = sexp.getArg_Val(); }
  // if (sexp.hasOP()) { StringID str_OP = sexp.getOP(); }

  return;
}

} // namespace IRI_STRUCTURAL
