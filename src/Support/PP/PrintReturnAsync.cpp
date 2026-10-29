// Generated Stub for IRI_TAG::ReturnAsync
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ReturnAsync
 * Meta:    STMT
 * Arguments:
 *   [0] IRID RetVal -> sexp.getArg_RetVal()
 * Flags: (none)
 */
std::string printReturnAsync(const PPContext &pp, IRID node) {
  IRI_GEN::ReturnAsyncSEXP sexp(node, pp.ctx);
  return "return.async " + ppRVal(pp, sexp.getArg_RetVal());
}

} // namespace IRI_STRUCTURAL
