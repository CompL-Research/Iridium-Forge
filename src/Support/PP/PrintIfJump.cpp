// Generated Stub for IRI_TAG::IfJump
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: IfJump
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Test -> sexp.getArg_Test()
 * Flags:
 *   - void   NOT -> sexp.hasNOT()
 *   - double IDX -> sexp.getIDX()
 */
std::string printIfJump(const PPContext &pp, IRID node) {
  IRI_GEN::IfJumpSEXP sexp(node, pp.ctx);
  // if x goto BB1 (if_true) / if not x goto BB1 (if_false); falls through.
  std::string neg = sexp.hasNOT() ? "not " : "";
  return "if " + neg + ppOperand(pp, sexp.getArg_Test()) + " goto " +
         ppBB(sexp.getIDX());
}

} // namespace IRI_STRUCTURAL
