// Generated Stub for IRI_TAG::JSDefineObjProp
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSDefineObjProp
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID Key -> sexp.getArg_Key()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags: (none)
 */
std::string printJSDefineObjProp(const PPContext &pp, IRID node) {
  // Define (CreateDataProperty), not Set: o.x := v
  IRI_GEN::JSDefineObjPropSEXP sexp(node, pp.ctx);
  return ppMember(pp, sexp.getArg_TargetObj(), sexp.getArg_Key()) + " := " +
         ppRVal(pp, sexp.getArg_Value());
}

} // namespace IRI_STRUCTURAL
