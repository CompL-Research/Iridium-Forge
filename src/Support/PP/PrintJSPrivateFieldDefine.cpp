// Generated Stub for IRI_TAG::JSPrivateFieldDefine
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSPrivateFieldDefine
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags: (none)
 */
std::string printJSPrivateFieldDefine(const PPContext &pp, IRID node) {
  // o.#[k] := v (PrivateFieldAdd: define_private_field)
  IRI_GEN::JSPrivateFieldDefineSEXP sexp(node, pp.ctx);
  return ppPrivateMember(pp, sexp.getArg_Obj(), sexp.getArg_Field()) + " := " +
         ppRVal(pp, sexp.getArg_Value());
}

} // namespace IRI_STRUCTURAL
