// Generated Stub for IRI_TAG::InvokeFinalizer
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: InvokeFinalizer
 * Meta:    STMT
 * Arguments: (none)
 * Flags:
 *   - double IDX -> sexp.getIDX()
 */
std::string printInvokeFinalizer(const PPContext &pp, IRID node) {
  IRI_GEN::InvokeFinalizerSEXP sexp(node, pp.ctx);
  return "finally.invoke " + ppBB(sexp.getIDX());
}

} // namespace IRI_STRUCTURAL
