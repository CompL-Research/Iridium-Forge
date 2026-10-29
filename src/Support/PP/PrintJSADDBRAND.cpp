// Generated Stub for IRI_TAG::JSADDBRAND
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSADDBRAND
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 *   [1] IRID HomeObj -> sexp.getArg_HomeObj()
 * Flags: (none)
 */
std::string printJSADDBRAND(const PPContext &pp, IRID node) {
  IRI_GEN::JSADDBRANDSEXP sexp(node, pp.ctx);
  return ppOperand(pp, sexp.getArg_Obj()) + ".[[Brands]] += " +
         ppRVal(pp, sexp.getArg_HomeObj());
}

} // namespace IRI_STRUCTURAL
