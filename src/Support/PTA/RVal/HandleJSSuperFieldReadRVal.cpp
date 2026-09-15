// Generated Stub for IRI_TAG::JSSuperFieldRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSuperFieldRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID This -> sexp.getArg_This()
 *   [1] IRID Super -> sexp.getArg_Super()
 *   [2] IRID Field -> sexp.getArg_Field()
 * Flags: (none)
 */
void computeJSSuperFieldReadVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSSuperFieldRead");
  // IRI_GEN::JSSuperFieldReadSEXP sexp(node, ptactx.ctx);

  // === TODO : JSSuperFieldRead ===
  // if (sexp.hasArg_This()) { IRID arg_This = sexp.getArg_This(); }
  // if (sexp.hasArg_Super()) { IRID arg_Super = sexp.getArg_Super(); }
  // if (sexp.hasArg_Field()) { IRID arg_Field = sexp.getArg_Field(); }

  return;
}

} // namespace IRI_STRUCTURAL
