// Generated Stub for IRI_TAG::Goto
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Goto
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - void   Deferred -> sexp.hasDeferred()
 *   - double IDX -> sexp.getIDX()
 */
std::string printGoto(const PPContext &pp, IRID node) {
  IRI_GEN::GotoSEXP sexp(node, pp.ctx);
  // deferred: the CFG split this block after an await (suspension point).
  std::string out = "goto " + ppBB(sexp.getIDX());
  return sexp.hasDeferred() ? "deferred " + out : out;
}

} // namespace IRI_STRUCTURAL
