// Generated Stub for IRI_TAG::StackToHeap
#include "Generated/IridiumTypes.h"
#include "Support/PP.hpp"
#include "Support/PP/PPHandlers.hpp"
#include "Support/PP/PPRVALDispatch.hpp"
#include <sstream>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: StackToHeap
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
std::string printStackToHeap(const PPContext &pp, IRID node) {
  // toheap(x, y): close_loc each binding, so closures keep their own copy
  // (e.g. per loop iteration)
  std::string out = "toheap(";
  auto a = pp.args(node);
  for (size_t i = 0; i < a.size(); ++i)
    out += (i ? ", " : "") + ppBinding(pp, a[i]);
  return out + ")";
}

} // namespace IRI_STRUCTURAL
