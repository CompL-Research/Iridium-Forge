// Generated Stub for IRI_TAG::JSDefineGetter
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSDefineGetter
 * Meta:    STMT
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID Key -> sexp.getArg_Key()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   NOENUM -> sexp.hasNOENUM()
 */
std::string printJSDefineGetter(const PPContext &pp, IRID node) {
  // [noenum] o.k := get λ f@BB (class members are not enumerable: noenum)
  IRI_GEN::JSDefineGetterSEXP sexp(node, pp.ctx);
  return std::string(sexp.hasNOENUM() ? "noenum " : "") +
         ppMember(pp, sexp.getArg_TargetObj(), sexp.getArg_Key()) + " := get " +
         ppRVal(pp, sexp.getArg_Value());
}

} // namespace IRI_STRUCTURAL
