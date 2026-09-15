// Generated Stub for IRI_TAG::IDOP (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: IDOP
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   PREFIX -> sexp.getPREFIX()
 *   - bool   INCREMENT -> sexp.getINCREMENT()
 */
void computeIDOPVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case IDOP");
  // IRI_GEN::IDOPSEXP sexp(node, ptactx.ctx);

  // === TODO : IDOP ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasPREFIX()) { bool val_PREFIX = sexp.getPREFIX(); }
  // if (sexp.hasINCREMENT()) { bool val_INCREMENT = sexp.getINCREMENT(); }

  return;
}

} // namespace IRI_STRUCTURAL
