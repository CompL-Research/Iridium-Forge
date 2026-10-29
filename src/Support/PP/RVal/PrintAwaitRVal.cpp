// Generated Stub for IRI_TAG::Await (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Await
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printAwaitRVal(const PPContext &pp, IRID node) {
  IRI_GEN::AwaitSEXP sexp(node, pp.ctx);
  return "await " + ppOperand(pp, sexp.getArg_Obj());
}

} // namespace IRI_STRUCTURAL
