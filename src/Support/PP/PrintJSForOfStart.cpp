// Generated Stub for IRI_TAG::JSForOfStart
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForOfStart
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printJSForOfStart(const PPContext &pp, IRID node) {
  // forof.start(x): the iterator state stays on the stack until forof.close
  IRI_GEN::JSForOfStartSEXP sexp(node, pp.ctx);
  return "forof.start(" + ppRVal(pp, sexp.getArg_Obj()) + ")";
}

} // namespace IRI_STRUCTURAL
