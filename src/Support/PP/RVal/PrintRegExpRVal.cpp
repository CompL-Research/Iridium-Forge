// Generated Stub for IRI_TAG::RegExp (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: RegExp
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string EXP -> sexp.getEXP()
 *   - string FLAGS -> sexp.getFLAGS()
 */
std::string printRegExpRVal(const PPContext &pp, IRID node) {
  IRI_GEN::RegExpSEXP sexp(node, pp.ctx);
  return "/" + std::string(pp.str(sexp.getEXP())) + "/" +
         std::string(sexp.hasFLAGS() ? pp.str(sexp.getFLAGS()) : "");
}

} // namespace IRI_STRUCTURAL
