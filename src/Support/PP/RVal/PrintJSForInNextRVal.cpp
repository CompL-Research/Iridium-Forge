// Generated Stub for IRI_TAG::JSForInNext (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForInNext
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID IteratorObj -> sexp.getArg_IteratorObj()
 * Flags: (none)
 */
std::string printJSForInNextRVal(const PPContext &pp, IRID node) {
  IRI_GEN::JSForInNextSEXP sexp(node, pp.ctx);
  return "forin.next(" + ppRVal(pp, sexp.getArg_IteratorObj()) + ")";
}

} // namespace IRI_STRUCTURAL
