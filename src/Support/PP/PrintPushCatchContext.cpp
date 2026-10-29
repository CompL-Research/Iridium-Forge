// Generated Stub for IRI_TAG::PushCatchContext
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PushCatchContext
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - double IDX -> sexp.getIDX()
 */
std::string printPushCatchContext(const PPContext &pp, IRID node) {
  IRI_GEN::PushCatchContextSEXP sexp(node, pp.ctx);
  return "try.push " + ppBB(sexp.getIDX());
}

} // namespace IRI_STRUCTURAL
