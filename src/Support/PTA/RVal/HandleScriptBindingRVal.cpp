// Generated Stub for IRI_TAG::ScriptBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ScriptBinding
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - void   JSLET -> sexp.hasJSLET()
 *   - void   JSCONST -> sexp.hasJSCONST()
 *   - void   JSVAR -> sexp.hasJSVAR()
 *   - string NAME -> sexp.getNAME()
 *   - double LINK -> sexp.getLINK()
 */
void computeScriptBindingVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case ScriptBinding");
  // IRI_GEN::ScriptBindingSEXP sexp(node, ptactx.ctx);

  // === TODO : ScriptBinding ===
  // bool has_JSLET = sexp.hasJSLET();
  // bool has_JSCONST = sexp.hasJSCONST();
  // bool has_JSVAR = sexp.hasJSVAR();
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }
  // if (sexp.hasLINK()) { double dbl_LINK = sexp.getLINK(); }

  return;
}

} // namespace IRI_STRUCTURAL
