#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTARVALHandlers.hpp"
#include "external/Prakriti.hpp"
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

void computeGlobalBindingVals(const PTAStatementContext &ptactx, IRID node,
                              std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PKR Unreachable :: GlobalBindingSEXP");
}

} // namespace IRI_STRUCTURAL
