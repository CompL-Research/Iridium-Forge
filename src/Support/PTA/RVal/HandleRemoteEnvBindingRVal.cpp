// Generated Stub for IRI_TAG::RemoteEnvBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: RemoteEnvBinding
 * Meta:    RVAL
 * Arguments:
 *   [0] IRID ParentReference -> sexp.getArg_ParentReference()
 * Flags:
 *   - void   MODULE -> sexp.hasMODULE()
 *   - void   MODULEI -> sexp.hasMODULEI()
 *   - void   MODULENSI -> sexp.hasMODULENSI()
 *   - double REFIDX -> sexp.getREFIDX()
 *   - double LINK -> sexp.getLINK()
 */
void computeRemoteEnvBindingVals(const PTAStatementContext &ptactx, IRID node,
                       std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PTA RVal unhandled case RemoteEnvBinding");
  // IRI_GEN::RemoteEnvBindingSEXP sexp(node, ptactx.ctx);

  // === TODO : RemoteEnvBinding ===
  // if (sexp.hasArg_ParentReference()) { IRID arg_ParentReference = sexp.getArg_ParentReference(); }
  // bool has_MODULE = sexp.hasMODULE();
  // bool has_MODULEI = sexp.hasMODULEI();
  // bool has_MODULENSI = sexp.hasMODULENSI();
  // if (sexp.hasREFIDX()) { double dbl_REFIDX = sexp.getREFIDX(); }
  // if (sexp.hasLINK()) { double dbl_LINK = sexp.getLINK(); }

  return;
}

} // namespace IRI_STRUCTURAL
