#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "external/Prakriti.hpp"

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForOfStart
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   AWAIT -> sexp.getAWAIT()
 */
void handleJSForOfStart(const PTAStatementContext &ptactx) {
  // Basically jusa no-op
  Prakriti::TraceHelperAuto th("JSForOfStart", ptactx.stmt.id);
}

} // namespace IRI_STRUCTURAL
