// Generated Stub for IRI_TAG::JSCheckBrand (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCheckBrand
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Method -> sexp.getArg_Method()
 * Flags: (none)
 */
void computeJSCheckBrandVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSCheckBrand");
  // IRI_GEN::JSCheckBrandSEXP sexp(node, ptactx.ctx);

  // === TODO : JSCheckBrand ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasArg_Method()) { IRID arg_Method = sexp.getArg_Method(); }

  return;
}

} // namespace IRI_STRUCTURAL
