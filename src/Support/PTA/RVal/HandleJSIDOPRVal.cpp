// Generated Stub for IRI_TAG::JSIDOP (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSIDOP
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - bool   PREFIX -> sexp.getPREFIX()
 *   - bool   INCREMENT -> sexp.getINCREMENT()
 */
void computeJSIDOPVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case JSIDOP");
  // IRI_GEN::JSIDOPSEXP sexp(node, ptactx.ctx);

  // === TODO : JSIDOP ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // if (sexp.hasPREFIX()) { bool val_PREFIX = sexp.getPREFIX(); }
  // if (sexp.hasINCREMENT()) { bool val_INCREMENT = sexp.getINCREMENT(); }

  return;
}

} // namespace IRI_STRUCTURAL
