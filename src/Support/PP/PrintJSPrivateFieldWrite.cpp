// Generated Stub for IRI_TAG::JSPrivateFieldWrite
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSPrivateFieldWrite
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Field -> sexp.getArg_Field()
 *   [2] IRID Value -> sexp.getArg_Value()
 * Flags:
 */
std::string printJSPrivateFieldWrite(const PPContext &pp, IRID node) {
  IRI_GEN::JSPrivateFieldWriteSEXP sexp(node, pp.ctx);
  // o.#[k] = v (PrivateSet)
  return ppPrivateMember(pp, sexp.getArg_Obj(), sexp.getArg_Field()) + " = " +
         ppRVal(pp, sexp.getArg_Value());
}

} // namespace IRI_STRUCTURAL
