// Generated Stub for IRI_TAG::JSComputedFieldRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSComputedFieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 */
std::string printJSComputedFieldReadRVal(const PPContext &pp, IRID node) {
  // o[topropkey(k)]: the key is cast to a property key. SAFE marks the cast
  // as redundant: o[k]
  IRI_GEN::JSComputedFieldReadSEXP sexp(node, pp.ctx);
  std::string key = ppRVal(pp, sexp.getArg_Field());
  if (!sexp.hasSAFE())
    key = "topropkey(" + key + ")";
  // With a receiver (super.f): receiver->obj.f, the access runs with
  // this = receiver
  std::string recv = sexp.hasArg_Receiver()
                         ? ppOperand(pp, sexp.getArg_Receiver()) + "->"
                         : "";
  return recv + ppOperand(pp, sexp.getArg_Obj()) + "[" + key + "]";
}

} // namespace IRI_STRUCTURAL
