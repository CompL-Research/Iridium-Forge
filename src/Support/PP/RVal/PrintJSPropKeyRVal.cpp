// Generated Stub for IRI_TAG::JSPropKey (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSPropKey
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Value -> sexp.getArg_Value()
 * Flags: (none)
 */
std::string printJSPropKeyRVal(const PPContext &pp, IRID node) {
  // IRI_GEN::JSPropKeySEXP sexp(node, pp.ctx);

  // === TODO : JSPropKey ===
  // std::string Value = ppOperand(pp, sexp.getArg_Value());

  return ppGeneric(pp, node);
}

} // namespace IRI_STRUCTURAL
