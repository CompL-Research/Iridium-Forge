// Generated Stub for IRI_TAG::JSAppend (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSAppend
 * Meta:    AMP
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID InsertionIdx -> sexp.getArg_InsertionIdx()
 *   [2] IRID SpreadObj -> sexp.getArg_SpreadObj()
 * Flags: (none)
 */
std::string printJSAppendRVal(const PPContext &pp, IRID node) {
  IRI_GEN::JSAppendSEXP sexp(node, pp.ctx);
  // append(target: arr, at: i, spread: x): pushes the spread values at i
  return "append(target: " + ppRVal(pp, sexp.getArg_TargetObj()) +
         ", at: " + ppRVal(pp, sexp.getArg_InsertionIdx()) +
         ", spread: " + ppRVal(pp, sexp.getArg_SpreadObj()) + ")";
}

} // namespace IRI_STRUCTURAL
