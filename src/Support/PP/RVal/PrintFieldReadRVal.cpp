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
  return ppMember(pp, sexp.getArg_Obj(), sexp.getArg_Field());
}

} // namespace IRI_STRUCTURAL
