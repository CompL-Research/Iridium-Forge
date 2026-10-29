// Generated Stub for IRI_TAG::JSCopyDataProperties
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCopyDataProperties
 * Meta:    AMP
 * Arguments:
 *   [0] IRID ExclusionObj -> sexp.getArg_ExclusionObj()
 *   [1] IRID SourceObj -> sexp.getArg_SourceObj()
 *   [2] IRID TargetObj -> sexp.getArg_TargetObj()
 * Flags: (none)
 *
 * AMP node used as a bare statement: print it as an expression statement.
 */
std::string printJSCopyDataProperties(const PPContext &pp, IRID node) {
  return ppRVal(pp, node);
}

} // namespace IRI_STRUCTURAL
