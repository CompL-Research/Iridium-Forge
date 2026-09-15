// Generated Stub for IRI_TAG::PVTEnvRead (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PVTEnvRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - void   SYMBOL -> sexp.hasSYMBOL()
 *   - void   METHOD -> sexp.hasMETHOD()
 *   - void   FULLY_RESOLVE -> sexp.hasFULLY_RESOLVE()
 */
void computePVTEnvReadVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case PVTEnvRead");
  // IRI_GEN::PVTEnvReadSEXP sexp(node, ptactx.ctx);

  // === TODO : PVTEnvRead ===
  // if (sexp.hasArg_Obj()) { IRID arg_Obj = sexp.getArg_Obj(); }
  // bool has_SYMBOL = sexp.hasSYMBOL();
  // bool has_METHOD = sexp.hasMETHOD();
  // bool has_FULLY_RESOLVE = sexp.hasFULLY_RESOLVE();

  return;
}

} // namespace IRI_STRUCTURAL
