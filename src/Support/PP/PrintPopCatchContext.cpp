// Generated Stub for IRI_TAG::PopCatchContext
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: PopCatchContext
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
std::string printPopCatchContext(const PPContext &pp, IRID node) {
  return "try.pop";
}

} // namespace IRI_STRUCTURAL
