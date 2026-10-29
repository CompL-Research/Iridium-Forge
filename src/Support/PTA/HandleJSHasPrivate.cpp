// Generated Stub for IRI_TAG::JSHasPrivate
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSHasPrivate
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID Name -> sexp.getArg_Name()
 * Flags: (none)
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handleJSHasPrivate(const PTAStatementContext &ptactx) {
  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
