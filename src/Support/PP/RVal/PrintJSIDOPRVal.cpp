// Generated Stub for IRI_TAG::JSIDOP (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSIDOP
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   PREFIX -> sexp.getPREFIX()
 *   - bool   INCREMENT -> sexp.getINCREMENT()
 */
std::string printJSIDOPRVal(const PPContext &pp, IRID node) {
  using S = IRI_GEN::JSIDOPSEXP;
  S sexp(node, pp.ctx);
  std::string op = sexp.hasINCREMENT() && sexp.getINCREMENT() ? "++" : "--";
  std::string obj = ppOperand(pp, sexp.getArg_Obj());
  return sexp.hasPREFIX() && sexp.getPREFIX() ? op + obj : obj + op;
}

} // namespace IRI_STRUCTURAL
