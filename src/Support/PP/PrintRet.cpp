// Generated Stub for IRI_TAG::Ret
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Ret
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
std::string printRet(const PPContext &pp, IRID node) {
  // OP_ret: leave a finally block, back to its finally.invoke (gosub) site.
  return "finally.ret";
}

} // namespace IRI_STRUCTURAL
