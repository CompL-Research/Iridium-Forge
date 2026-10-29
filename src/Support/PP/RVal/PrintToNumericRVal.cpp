// Generated Stub for IRI_TAG::ToNumeric (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ToNumeric
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printToNumericRVal(const PPContext &pp, IRID node) {
  IRI_GEN::ToNumericSEXP sexp(node, pp.ctx);
  return "tonumeric(" + ppRVal(pp, sexp.getArg_Obj()) + ")";
}

} // namespace IRI_STRUCTURAL
