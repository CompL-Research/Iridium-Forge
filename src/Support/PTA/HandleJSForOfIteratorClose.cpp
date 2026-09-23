#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "external/Prakriti.hpp"

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSForOfIteratorClose
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleJSForOfIteratorClose(const PTAStatementContext &ptactx) {
  // This will need to call return() at some point, but we are actively ignoring
  // support for iterables that have a return()
  Prakriti::TraceHelperAuto th("JSForOfIteratorClose", ptactx.stmt.id);
}

} // namespace IRI_STRUCTURAL
