// Generated Stub for IRI_TAG::JSBinop
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSBinop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID LBinop -> sexp.getArg_LBinop()
 *   [1] IRID RBinop -> sexp.getArg_RBinop()
 * Flags:
 *   - string OP -> sexp.getOP()
 *
 * AMP node used as a bare statement: evaluate it via RVal resolution for
 * its side effects and discard the produced value.
 */
void handleJSBinop(const PTAStatementContext &ptactx) {
  std::set<Prakriti::NodeUID> discarded;
  resolvePKRRVal(ptactx, ptactx.stmt.id, discarded);
}

} // namespace IRI_STRUCTURAL
