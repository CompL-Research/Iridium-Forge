// Generated Stub for IRI_TAG::ModuleDecl (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ModuleDecl
 * Meta:    RVAL
 * Arguments: (none)
 * Flags: (none)
 */
std::string printModuleDeclRVal(const PPContext &pp, IRID node) {
  // Truthy only while the module function runs its declaration pass.
  return "moduleDecl";
}

} // namespace IRI_STRUCTURAL
