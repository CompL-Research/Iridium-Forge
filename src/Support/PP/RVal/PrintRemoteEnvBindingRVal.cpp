// Generated Stub for IRI_TAG::RemoteEnvBinding (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

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
std::string printRemoteEnvBindingRVal(const PPContext &pp, IRID node) {
  return ppBinding(pp, node);
}

} // namespace IRI_STRUCTURAL
