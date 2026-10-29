// Generated Stub for IRI_TAG::JSArray (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSArray
 * Meta:    RVAL
 * Arguments: (none)
 * Flags: (none)
 */
std::string printJSArrayRVal(const PPContext &pp, IRID node) {
  return "[" + ppList(pp, pp.args(node)) + "]";
}

} // namespace IRI_STRUCTURAL
