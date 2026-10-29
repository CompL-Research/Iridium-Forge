// Generated Stub for IRI_TAG::JSCheckBrand (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCheckBrand
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Method -> sexp.getArg_Method()
 * Flags: (none)
 */
std::string printJSCheckBrandRVal(const PPContext &pp, IRID node) {
  // o.#method[m]: brand check on o, yields the private method m
  IRI_GEN::JSCheckBrandSEXP sexp(node, pp.ctx);
  return ppOperand(pp, sexp.getArg_Obj()) + ".#method[" +
         ppRVal(pp, sexp.getArg_Method()) + "]";
}

} // namespace IRI_STRUCTURAL
