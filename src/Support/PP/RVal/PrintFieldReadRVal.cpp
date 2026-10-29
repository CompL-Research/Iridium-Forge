// Generated Stub for IRI_TAG::FieldRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: FieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
std::string printFieldReadRVal(const PPContext &pp, IRID node) {
  IRI_GEN::FieldReadSEXP sexp(node, pp.ctx);
  // With a receiver (super.f): receiver->obj.f, the access runs with
  // this = receiver
  std::string recv = sexp.hasArg_Receiver()
                         ? ppOperand(pp, sexp.getArg_Receiver()) + "->"
                         : "";
  return recv + ppMember(pp, sexp.getArg_Obj(), sexp.getArg_Field());
}

} // namespace IRI_STRUCTURAL
