#include "Support/BBSupport.hpp"
#include "Generated/IridiumTypes.h"
#include "Support/IndexedIterator.hpp"
#include <vector>

namespace IRI_STRUCTURAL {

using namespace IRI_GEN;
using namespace IRI_STORAGE;
using ITR_RET = IndexedIterator<std::vector<IRID>>;

ITR_RET BBSupport::stmts() const {
  return IndexedIterator(std::move(pool->get_args(id)));
}

std::vector<IRID> BBSupport::stmtsVec() const {
  return pool->get_args(id);
}

} // namespace IRI_STRUCTURAL
