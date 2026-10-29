// Generated Stub for IRI_TAG::JSSetName
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSSetName
 * Meta:    STMT
 * Arguments:
 *   [0] IRID obj -> sexp.getArg_obj()
 *   [1] IRID name -> sexp.getArg_name()
 * Flags: (none)
 */
std::string printJSSetName(const PPContext &pp, IRID node) {
  IRI_GEN::JSSetNameSEXP sexp(node, pp.ctx);
  return ppOperand(pp, sexp.getArg_obj()) + ".[[Name]] = " +
         ppRVal(pp, sexp.getArg_name());
}

} // namespace IRI_STRUCTURAL
