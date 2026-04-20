#include "Support/BBContainerSupport.hpp"
#include "Generated/IridiumTypes.h"
#include "Support/BBSupport.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {

using namespace IRI_GEN;
using namespace IRI_STORAGE;
using ITR_RET = IndexedIterator<std::vector<IRID>>;

ITR_RET BBContainerSupport::bbs() const {
  IRID bbID = getArg_BB();
  ListSEXP bbs(bbID, *pool); // Basically just for the assertion
  return IndexedIterator(std::move(pool->get_args(bbID)));
}

IRID BBContainerSupport::getBBByIDX(double idx) {
  IRID bbID = getArg_BB();
  ListSEXP bbs(bbID, *pool);
  auto argsView = pool->get_args_view(bbID);
  for (auto bbID : argsView) {
    BBSupport bb(bbID, *pool);
    if (bb.getIDX() == idx) return bbID;
  }
  throw std::runtime_error("Failed to fetch BB By ID");
}

} // namespace IRI_STRUCTURAL
