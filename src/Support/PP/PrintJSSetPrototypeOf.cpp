// Generated Stub for IRI_TAG::JSSetPrototypeOf
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSetPrototypeOf
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID ProtoValue -> sexp.getArg_ProtoValue()
 * Flags: (none)
 */
std::string printJSSetPrototypeOf(const PPContext &pp, IRID node) {
  IRI_GEN::JSSetPrototypeOfSEXP sexp(node, pp.ctx);
  return ppOperand(pp, sexp.getArg_TargetObj()) + ".[[Prototype]] = " +
         ppRVal(pp, sexp.getArg_ProtoValue());
}

} // namespace IRI_STRUCTURAL
