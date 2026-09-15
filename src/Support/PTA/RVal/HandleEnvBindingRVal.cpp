// Generated Stub for IRI_TAG::EnvBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: EnvBinding
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - void   JSARG -> sexp.hasJSARG()
 *   - void   JSRESTARG -> sexp.hasJSRESTARG()
 *   - void   JSLET -> sexp.hasJSLET()
 *   - void   JSCONST -> sexp.hasJSCONST()
 *   - void   JSVAR -> sexp.hasJSVAR()
 *   - string NAME -> sexp.getNAME()
 *   - double REFIDX -> sexp.getREFIDX()
 *   - double SCOPE -> sexp.getSCOPE()
 *   - double NEXT -> sexp.getNEXT()
 *   - double LINK -> sexp.getLINK()
 */
void computeEnvBindingVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case EnvBinding");
  // IRI_GEN::EnvBindingSEXP sexp(node, ptactx.ctx);

  // === TODO : EnvBinding ===
  // bool has_JSARG = sexp.hasJSARG();
  // bool has_JSRESTARG = sexp.hasJSRESTARG();
  // bool has_JSLET = sexp.hasJSLET();
  // bool has_JSCONST = sexp.hasJSCONST();
  // bool has_JSVAR = sexp.hasJSVAR();
  // if (sexp.hasNAME()) { StringID str_NAME = sexp.getNAME(); }
  // if (sexp.hasREFIDX()) { double dbl_REFIDX = sexp.getREFIDX(); }
  // if (sexp.hasSCOPE()) { double dbl_SCOPE = sexp.getSCOPE(); }
  // if (sexp.hasNEXT()) { double dbl_NEXT = sexp.getNEXT(); }
  // if (sexp.hasLINK()) { double dbl_LINK = sexp.getLINK(); }

  return;
}

} // namespace IRI_STRUCTURAL
