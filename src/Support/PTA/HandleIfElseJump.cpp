#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <set>

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
void handleIfElseJump(const PTAStatementContext &ptactx) {
  Prakriti::TraceHelperAuto th("IfElseJump", ptactx.stmt.id);

  IRI_GEN::IfElseJumpSEXP sexp(ptactx.stmt.id, ptactx.ctx);
  std::set<Prakriti::NodeUID> discarded;
  {
    Prakriti::TraceHelperAuto th("IfElseJump::Test", sexp.getArg_Test());
    resolvePKRRVal(ptactx, sexp.getArg_Test(), discarded);
  }
}

} // namespace IRI_STRUCTURAL
