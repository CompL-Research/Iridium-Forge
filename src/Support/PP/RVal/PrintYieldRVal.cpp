// Generated Stub for IRI_TAG::Yield (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Yield
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printYieldRVal(const PPContext &pp, IRID node) {
  IRI_GEN::YieldSEXP sexp(node, pp.ctx);
  return "yield " + ppOperand(pp, sexp.getArg_Obj());
}

} // namespace IRI_STRUCTURAL
