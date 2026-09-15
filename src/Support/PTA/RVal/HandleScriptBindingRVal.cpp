#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <set>

namespace IRI_STRUCTURAL {

void computeScriptBindingVals(const PTAStatementContext &ptactx, IRID node,
                              std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PKR Unreachable :: ScriptBindingSEXP");
}

} // namespace IRI_STRUCTURAL
