#include "Support/FileSupport.hpp"
#include "Generated/IridiumTypes.h"
#include "Storage/StringPool.h"
#include "Support/IndexedIterator.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using ITR_RET = IndexedIterator<std::vector<IRID>>;

ITR_RET FileSupport::containers() const {
  std::span<const IRID> allIDX = pool->get_args_view(id);
  std::span<const IRID> sub = allIDX.subspan(4);
  std::vector<IRID> targetIDXs(sub.begin(), sub.end());

  return IndexedIterator(std::move(targetIDXs));
}

inline ITR_RET getIt(IridiumPool * pool, IRID fileID, int offset, StringID type) {
  IRID moduleRequestsIDX = pool->get_args_view(fileID)[offset];
  ListSEXP moduleRequests(moduleRequestsIDX, *pool);
  if (moduleRequests.getTYPE() != type)
    throw std::runtime_error("[Forge]: Failed to get module vec, typecheck failed!");

  return IndexedIterator(std::move(pool->get_args(moduleRequestsIDX)));
}

ITR_RET FileSupport::moduleRequests() const {
  return getIt(pool, id, 0, pool->strings.intern("ModuleRequest"));
}


ITR_RET FileSupport::staticImports() const {
  return getIt(pool, id, 1, pool->strings.intern("StaticImport"));
}

ITR_RET FileSupport::staticExports() const {
  return getIt(pool, id, 2, pool->strings.intern(""));
}


ITR_RET FileSupport::staticStarExports() const {
  return getIt(pool, id, 3, pool->strings.intern("StarExport"));
}

} // namespace IRI_STRUCTURAL
