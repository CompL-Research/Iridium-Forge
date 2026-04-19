#include "Support/BBContainerSupport.hpp"
#include "Generated/IridiumTypes.h"

namespace IRI_STRUCTURAL {

using namespace IRI_GEN;
using namespace IRI_STORAGE;
using ITR_RET = IndexedIterator<std::vector<IRID>>;

ITR_RET BBContainerSupport::bbs() const {
  BBContainerSEXP bbCont(id, *pool);
  IRID bbID = bbCont.getArg_BB();
  ListSEXP bbs(bbCont.getArg_BB(), *pool);
  return IndexedIterator(std::move(pool->get_args(bbID)));
}

} // namespace IRI_STRUCTURAL
