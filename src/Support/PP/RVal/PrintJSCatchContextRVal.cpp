// Generated Stub for IRI_TAG::JSCatchContext (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCatchContext
 * Meta:    AMP
 * Arguments: (none)
 * Flags: (none)
 */
std::string printJSCatchContextRVal(const PPContext &pp, IRID node) {
  // The caught exception value (already on the stack in the handler).
  return "caught";
}

} // namespace IRI_STRUCTURAL
