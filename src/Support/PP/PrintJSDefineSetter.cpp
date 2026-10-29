// Generated Stub for IRI_TAG::JSDefineSetter
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSDefineSetter
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID Key -> sexp.getArg_Key()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   NOENUM -> sexp.hasNOENUM()
 */
std::string printJSDefineSetter(const PPContext &pp, IRID node) {
  // [noenum] o.k := set λ f@BB (class members are not enumerable: noenum)
  IRI_GEN::JSDefineSetterSEXP sexp(node, pp.ctx);
  return std::string(sexp.hasNOENUM() ? "noenum " : "") +
         ppMember(pp, sexp.getArg_TargetObj(), sexp.getArg_Key()) + " := set " +
         ppRVal(pp, sexp.getArg_Value());
}

} // namespace IRI_STRUCTURAL
