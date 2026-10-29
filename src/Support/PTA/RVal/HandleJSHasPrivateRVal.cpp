// Generated Stub for IRI_TAG::JSHasPrivate (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSHasPrivate
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Name -> sexp.getArg_Name()
 * Flags: (none)
 */
void computeJSHasPrivateVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSHasPrivate");
  // IRI_GEN::JSHasPrivateSEXP sexp(node, ptactx.ctx);

  // === TODO : JSHasPrivate ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_Name()) { IRID arg_Name = sexp.getArg_Name(); }

  return;
}

} // namespace IRI_STRUCTURAL
