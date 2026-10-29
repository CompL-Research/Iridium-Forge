// Generated Stub for IRI_TAG::DCTRRet (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: DCTRRet
 * Meta:    AMP
 * Arguments:
 *   [0] IRID userObj -> sexp.getArg_userObj()
 *   [1] IRID thisObj -> sexp.getArg_thisObj()
 * Flags: (none)
 */
std::string printDCTRRetRVal(const PPContext &pp, IRID node) {
  // ctor.ret(value: v, this: t): a derived constructor's return; an object
  // value wins, otherwise `this` (check_ctor_return)
  IRI_GEN::DCTRRetSEXP sexp(node, pp.ctx);
  return "ctor.ret(value: " + ppRVal(pp, sexp.getArg_userObj()) +
         ", this: " + ppRVal(pp, sexp.getArg_thisObj()) + ")";
}

} // namespace IRI_STRUCTURAL
