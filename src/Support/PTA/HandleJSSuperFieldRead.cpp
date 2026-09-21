// Generated Stub for IRI_TAG::JSSuperFieldRead
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSuperFieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID This -> sexp.getArg_This()
 *   [1] IRID Super -> sexp.getArg_Super()
 *   [2] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handleJSSuperFieldRead(const PTAStatementContext &ptactx) {
  Prakriti::TraceHelperAuto th("JSSuperFieldRead", ptactx.stmt.id);

  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
