// Generated Stub for IRI_TAG::JSClass (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSClass
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID Constructor -> sexp.getArg_Constructor()
 *   [1] IRID Parent -> sexp.getArg_Parent()
 * Flags:
 *   - string NAME -> sexp.getNAME()
 */
std::string printJSClassRVal(const PPContext &pp, IRID node) {
  // class B extends P { ctor: <fn B @BB12> }; derived iff it has a parent
  IRI_GEN::JSClassSEXP sexp(node, pp.ctx);
  std::string name(sexp.hasNAME() ? pp.str(sexp.getNAME()) : "");
  std::string out = "class" + (name.empty() ? "" : " " + name);
  if (sexp.hasArg_Parent())
    out += " extends " + ppOperand(pp, sexp.getArg_Parent());
  return out + " { ctor: " + ppRVal(pp, sexp.getArg_Constructor()) + " }";
}

} // namespace IRI_STRUCTURAL
