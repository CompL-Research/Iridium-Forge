// Generated Stub for IRI_TAG::JSForAwaitOfStart
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForAwaitOfStart
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
void handleJSForAwaitOfStart(const PTAStatementContext &ptactx) {
  // A no-op, like JSForOfStart; the await step is unhandled (CompoundAssn)
  Prakriti::TraceHelperAuto th("JSForAwaitOfStart", ptactx.stmt.id);
}

} // namespace IRI_STRUCTURAL
