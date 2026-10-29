// Generated Stub for IRI_TAG::JSPrivate (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSPrivate
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string IridiumPrimitive -> sexp.getIridiumPrimitive()
 */
std::string printJSPrivateRVal(const PPContext &pp, IRID node) {
  IRI_GEN::JSPrivateSEXP sexp(node, pp.ctx);
  // OP_private_symbol: a fresh private symbol; the name already starts with #
  std::string name(pp.str(sexp.getIridiumPrimitive()));
  return name.starts_with("#") ? name : "#" + name;
}

} // namespace IRI_STRUCTURAL
