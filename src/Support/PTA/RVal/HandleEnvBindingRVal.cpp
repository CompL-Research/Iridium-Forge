#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include <cassert>
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

void computeEnvBindingVals(const PTAStatementContext &ptactx, IRID node,
                           std::set<Prakriti::NodeUID> &res_) {
  throw std::runtime_error("PKR Unreachable :: EnvBindingSEXP");
}

} // namespace IRI_STRUCTURAL
