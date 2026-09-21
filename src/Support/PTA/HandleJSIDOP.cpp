// Generated Stub for IRI_TAG::JSIDOP
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSIDOP
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   PREFIX -> sexp.getPREFIX()
 *   - bool   INCREMENT -> sexp.getINCREMENT()
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handleJSIDOP(const PTAStatementContext &ptactx) {
  Prakriti::TraceHelperAuto th("JSIDOP", ptactx.stmt.id);

  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
