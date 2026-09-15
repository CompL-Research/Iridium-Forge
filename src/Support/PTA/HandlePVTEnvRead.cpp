// Generated Stub for IRI_TAG::PVTEnvRead
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PVTEnvRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - void   SYMBOL -> sexp.hasSYMBOL()
 *   - void   METHOD -> sexp.hasMETHOD()
 *   - void   FULLY_RESOLVE -> sexp.hasFULLY_RESOLVE()
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handlePVTEnvRead(const PTAStatementContext &ptactx) {
  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
