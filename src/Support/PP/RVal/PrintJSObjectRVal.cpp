// Generated Stub for IRI_TAG::JSObject (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSObject
 * Meta:    RVAL
 * Arguments: (none)
 * Flags: (none)
 */
std::string printJSObjectRVal(const PPContext &pp, IRID node) {
  // A new empty object (OP_object); its properties are defined by separate
  // statements (JSDefineObjProp / JSDefineObjMethod)
  return "{}";
}

} // namespace IRI_STRUCTURAL
