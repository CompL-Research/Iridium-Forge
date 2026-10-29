// Generated Stub for IRI_TAG::JSForAwaitOfStart
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForAwaitOfStart
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printJSForAwaitOfStart(const PPContext &pp, IRID node) {
  // forawait.start(x): the iterator state stays on the stack until forof.close
  IRI_GEN::JSForAwaitOfStartSEXP sexp(node, pp.ctx);
  return "forawait.start(" + ppRVal(pp, sexp.getArg_Obj()) + ")";
}

} // namespace IRI_STRUCTURAL
