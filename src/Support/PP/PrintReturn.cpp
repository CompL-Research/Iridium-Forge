// Generated Stub for IRI_TAG::Return
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Return
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printReturn(const PPContext &pp, IRID node) {
  IRI_GEN::ReturnSEXP sexp(node, pp.ctx);
  return "return " + ppRVal(pp, sexp.getArg_Obj());
}

} // namespace IRI_STRUCTURAL
