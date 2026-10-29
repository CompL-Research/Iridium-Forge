// Generated Stub for IRI_TAG::JSPrivateFieldRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSPrivateFieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
std::string printJSPrivateFieldReadRVal(const PPContext &pp, IRID node) {
  IRI_GEN::JSPrivateFieldReadSEXP sexp(node, pp.ctx);
  return ppPrivateMember(pp, sexp.getArg_Obj(), sexp.getArg_Field());
}

} // namespace IRI_STRUCTURAL
