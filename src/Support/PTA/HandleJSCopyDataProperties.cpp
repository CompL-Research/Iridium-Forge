// Generated Stub for IRI_TAG::JSCopyDataProperties
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCopyDataProperties
 * Meta:    AMP
 * Arguments:
 *   [0] IRID ExclusionObj -> sexp.getArg_ExclusionObj()
 *   [1] IRID SourceObj -> sexp.getArg_SourceObj()
 *   [2] IRID TargetObj -> sexp.getArg_TargetObj()
 * Flags: (none)
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handleJSCopyDataProperties(const PTAStatementContext &ptactx) {
  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
