// Generated Stub for IRI_TAG::JSHasPrivate (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSHasPrivate
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Name -> sexp.getArg_Name()
 * Flags: (none)
 */
std::string printJSHasPrivateRVal(const PPContext &pp, IRID node) {
  // #[k] in o: does o have the private element named by k
  IRI_GEN::JSHasPrivateSEXP sexp(node, pp.ctx);
  return "#[" + ppRVal(pp, sexp.getArg_Name()) + "] in " +
         ppOperand(pp, sexp.getArg_Obj());
}

} // namespace IRI_STRUCTURAL
