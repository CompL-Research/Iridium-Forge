#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Goto
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - void   Deferred -> sexp.hasDeferred()
 *   - double IDX -> sexp.getIDX()
 */
void handleGoto(const PTAStatementContext &ptactx) {}

} // namespace IRI_STRUCTURAL
