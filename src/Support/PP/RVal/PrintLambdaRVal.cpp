// Generated Stub for IRI_TAG::Lambda (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Lambda
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - int32  ClosureScope -> sexp.getClosureScope()
 *   - uint32 ClosIDX -> sexp.getClosIDX()
 *   - uint32 REFIDX -> sexp.getREFIDX()
 */
std::string printLambdaRVal(const PPContext &pp, IRID node) {
  // c2 (the closure named by handle 2). Before the closure tree resolves refs
  // there is no handle yet, so fall back to the target's scope.
  IRI_GEN::LambdaSEXP sexp(node, pp.ctx);
  return sexp.hasClosIDX() ? "c" + std::to_string(sexp.getClosIDX())
                           : "s" + std::to_string(sexp.getClosureScope());
}

} // namespace IRI_STRUCTURAL
