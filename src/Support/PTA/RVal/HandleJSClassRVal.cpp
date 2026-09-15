// Generated Stub for IRI_TAG::JSClass (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSClass
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Parent -> sexp.getArg_Parent()
 *   [1] IRID Constructor -> sexp.getArg_Constructor()
 * Flags:
 *   - void   DERIVED -> sexp.hasDERIVED()
 *   - string NAME -> sexp.getNAME()
 */
void computeJSClassVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSClass");
  // IRI_GEN::JSClassSEXP sexp(node, ptactx.ctx);

  // === TODO : JSClass ===
  // if (sexp.hasArg_Parent()) { IRID arg_Parent = sexp.getArg_Parent(); }
  // if (sexp.hasArg_Constructor()) { IRID arg_Constructor = sexp.getArg_Constructor(); }
  // bool has_DERIVED = sexp.hasDERIVED();
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }

  return;
}

} // namespace IRI_STRUCTURAL
