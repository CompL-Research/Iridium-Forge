// Generated Stub for IRI_TAG::PropKeyCheck
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PropKeyCheck
 * Meta:    STMT
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags: (none)
 */
std::string printPropKeyCheck(const PPContext &pp, IRID node) {
  // IRI_GEN::PropKeyCheckSEXP sexp(node, pp.ctx);

  // === TODO : PropKeyCheck ===
  // std::string Obj = ppOperand(pp, sexp.getArg_Obj());

  return ppGeneric(pp, node);
}

} // namespace IRI_STRUCTURAL
