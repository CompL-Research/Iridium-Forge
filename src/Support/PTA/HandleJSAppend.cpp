// Generated Stub for IRI_TAG::JSAppend
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSAppend
 * Meta:    AMP
 * Arguments:
 *   [0] IRID TargetObj -> sexp.getArg_TargetObj()
 *   [1] IRID InsertionIdx -> sexp.getArg_InsertionIdx()
 *   [2] IRID SpreadObj -> sexp.getArg_SpreadObj()
 * Flags: (none)
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handleJSAppend(const PTAStatementContext &ptactx) {
  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
