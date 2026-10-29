// Generated Stub for IRI_TAG::FieldWrite
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: FieldWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags: (none)
 */
std::string printFieldWrite(const PPContext &pp, IRID node) {
  IRI_GEN::FieldWriteSEXP sexp(node, pp.ctx);
  // With a receiver (super.f): receiver->obj.f, the access runs with
  // this = receiver
  std::string recv = sexp.hasArg_Receiver()
                         ? ppOperand(pp, sexp.getArg_Receiver()) + "->"
                         : "";
  return recv + ppMember(pp, sexp.getArg_Obj(), sexp.getArg_Field()) + " = " +
         ppRVal(pp, sexp.getArg_Value());
}

} // namespace IRI_STRUCTURAL
