// Generated Stub for IRI_TAG::JSForOfNext (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForOfNext
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printJSForOfNextRVal(const PPContext &pp, IRID node) {
  // forof.next(iterable: x): steps the iterator on the stack; x is not read
  // by the lowering (kept for analyses)
  IRI_GEN::JSForOfNextSEXP sexp(node, pp.ctx);
  return "forof.next(iterable: " + ppRVal(pp, sexp.getArg_Obj()) + ")";
}

} // namespace IRI_STRUCTURAL
