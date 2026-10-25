// Generated Stub for IRI_TAG::RegExp (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: RegExp
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string EXP -> sexp.getEXP()
 *   - string FLAGS -> sexp.getFLAGS()
 */
void computeRegExpVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case RegExp");
  // IRI_GEN::RegExpSEXP sexp(node, ptactx.ctx);

  // === TODO : RegExp ===
  // if (sexp.hasEXP()) { StringID str_EXP = sexp.getEXP(); }
  // if (sexp.hasFLAGS()) { StringID str_FLAGS = sexp.getFLAGS(); }

  return;
}

} // namespace IRI_STRUCTURAL
