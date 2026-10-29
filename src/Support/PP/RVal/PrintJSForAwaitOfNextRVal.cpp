// Generated Stub for IRI_TAG::JSForAwaitOfNext (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForAwaitOfNext
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printJSForAwaitOfNextRVal(const PPContext &pp, IRID node) {
  // forawait.next(iterable: x): steps the iterator on the stack; x is not read
  // by the lowering (kept for analyses)
  IRI_GEN::JSForAwaitOfNextSEXP sexp(node, pp.ctx);
  return "forawait.next(iterable: " + ppRVal(pp, sexp.getArg_Obj()) + ")";
}

} // namespace IRI_STRUCTURAL
