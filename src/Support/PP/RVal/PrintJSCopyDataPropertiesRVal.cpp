// Generated Stub for IRI_TAG::JSCopyDataProperties (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCopyDataProperties
 * Meta:    AMP
 * Arguments:
 *   [0] IRID ExclusionObj -> sexp.getArg_ExclusionObj()
 *   [1] IRID SourceObj -> sexp.getArg_SourceObj()
 *   [2] IRID TargetObj -> sexp.getArg_TargetObj()
 * Flags: (none)
 */
std::string printJSCopyDataPropertiesRVal(const PPContext &pp, IRID node) {
  // copyprops(target: t, source: s, except: e); the value is the target
  IRI_GEN::JSCopyDataPropertiesSEXP sexp(node, pp.ctx);
  return "copyprops(target: " + ppRVal(pp, sexp.getArg_TargetObj()) +
         ", source: " + ppRVal(pp, sexp.getArg_SourceObj()) +
         ", except: " + ppRVal(pp, sexp.getArg_ExclusionObj()) + ")";
}

} // namespace IRI_STRUCTURAL
