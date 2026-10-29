// Generated Stub for IRI_TAG::PoolBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PoolBinding
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Lambda -> sexp.getArg_Lambda()
 * Flags:
 *   - double REFIDX -> sexp.getREFIDX()
 */
std::string printPoolBindingRVal(const PPContext &pp, IRID node) {
  IRI_GEN::PoolBindingSEXP sexp(node, pp.ctx);
  return ppRVal(pp, sexp.getArg_Lambda());
}

} // namespace IRI_STRUCTURAL
