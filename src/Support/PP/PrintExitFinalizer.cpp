// Generated Stub for IRI_TAG::ExitFinalizer
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ExitFinalizer
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
std::string printExitFinalizer(const PPContext &pp, IRID node) {
  // Leave a finally block early (break/continue/return) instead of via
  // finally.ret: drops the return address pushed by finally.invoke.
  return "finally.exit";
}

} // namespace IRI_STRUCTURAL
