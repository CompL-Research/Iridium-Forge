// Generated Stub for IRI_TAG::ModuleDeclarationEarlyExit
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: ModuleDeclarationEarlyExit
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
std::string printModuleDeclarationEarlyExit(const PPContext &pp, IRID node) {
  // The synchronous end of the module declaration pass. The standard
  // return/Goto that follows it is dead at runtime (the peephole strips it).
  return "module.earlyexit";
}

} // namespace IRI_STRUCTURAL
