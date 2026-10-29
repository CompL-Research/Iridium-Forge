// Generated Stub for IRI_TAG::IfElseJump
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: IfElseJump
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Test -> sexp.getArg_Test()
 * Flags:
 *   - void   NOT -> sexp.hasNOT()
 *   - double TRUE -> sexp.getTRUE()
 *   - double FALSE -> sexp.getFALSE()
 */
std::string printIfElseJump(const PPContext &pp, IRID node) {
  IRI_GEN::IfElseJumpSEXP sexp(node, pp.ctx);
  // if x goto BB1 else BB2 / if not x goto BB1 else BB2
  std::string neg = sexp.hasNOT() ? "not " : "";
  return "if " + neg + ppOperand(pp, sexp.getArg_Test()) + " goto " +
         ppBB(sexp.getTRUE()) + " else " + ppBB(sexp.getFALSE());
}

} // namespace IRI_STRUCTURAL
