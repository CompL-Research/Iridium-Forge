// Generated Stub for IRI_TAG::Apply
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Apply
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID Context -> sexp.getArg_Context()
 *   [2] IRID ArgList -> sexp.getArg_ArgList()
 * Flags:
 *   - void   ConstructorCall -> sexp.hasConstructorCall()
 *   - void   Super -> sexp.hasSuper()
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handleApply(const PTAStatementContext &ptactx) {
  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
