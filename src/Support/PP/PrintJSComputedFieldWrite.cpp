// Generated Stub for IRI_TAG::JSComputedFieldWrite
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSComputedFieldWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 */
std::string printJSComputedFieldWrite(const PPContext &pp, IRID node) {
  // o[topropkey(k)] = v: the key is cast (OP_to_propkey) before v is
  // evaluated. SAFE marks the cast as redundant: o[k] = v
  IRI_GEN::JSComputedFieldWriteSEXP sexp(node, pp.ctx);
  std::string key = ppRVal(pp, sexp.getArg_Field());
  if (!sexp.hasSAFE())
    key = "topropkey(" + key + ")";
  // With a receiver (super.f): receiver->obj.f, the access runs with
  // this = receiver
  std::string recv = sexp.hasArg_Receiver()
                         ? ppOperand(pp, sexp.getArg_Receiver()) + "->"
                         : "";
  return recv + ppOperand(pp, sexp.getArg_Obj()) + "[" + key + "] = " +
         ppRVal(pp, sexp.getArg_Value());
}

} // namespace IRI_STRUCTURAL
