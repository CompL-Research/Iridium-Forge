// Generated Stub for IRI_TAG::DCTRRet
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: DCTRRet
 * Meta:    AMP
 * Arguments:
 *   [0] IRID userObj -> sexp.getArg_userObj()
 *   [1] IRID thisObj -> sexp.getArg_thisObj()
 * Flags: (none)
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handleDCTRRet(const PTAStatementContext &ptactx) {
  Prakriti::TraceHelperAuto th("DCTRRet", ptactx.stmt.id);

  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
