#include "Support/FileSupport.hpp"
#include "Generated/IridiumTypes.h"
#include "Storage/StringPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/IndexedIterator.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using ITR_RET = IndexedIterator<std::vector<IRID>>;

ITR_RET FileSupport::containers() const {
  std::span<const IRID> allIDX = ctx->storage.nodes.get_args_view(id);
  std::span<const IRID> sub = allIDX.subspan(4);
  std::vector<IRID> targetIDXs(sub.begin(), sub.end());
  return IndexedIterator(std::move(targetIDXs));
}

inline ITR_RET getIt(IRIContext * ctx, IRID fileID, int offset, StringID type) {
  IRID moduleRequestsIDX = ctx->storage.nodes.get_args_view(fileID)[offset];
  ListSEXP moduleRequests(moduleRequestsIDX, *ctx);
  if (moduleRequests.getTYPE() != type)
    throw std::runtime_error("[Forge]: Failed to get module vec, typecheck failed!");

  return IndexedIterator(std::move(ctx->storage.nodes.get_args(moduleRequestsIDX)));
}

IRID FileSupport::moduleRequests() const {
  return ctx->storage.nodes.get_args_view(id)[0];
}


IRID FileSupport::staticImports() const {
  return ctx->storage.nodes.get_args_view(id)[1];
}

IRID FileSupport::staticExports() const {
  return ctx->storage.nodes.get_args_view(id)[2];
}


IRID FileSupport::staticStarExports() const {
  return ctx->storage.nodes.get_args_view(id)[3];
}

IRI_GEN::IRID FileSupport::getBBContainerByScopeIDX(double id) const {
  for (auto [cID, _] : containers()) {
    BBContainerSupport c(cID, *ctx);
    if (c.getScopeIDX() == id) return cID;
  }
  throw std::runtime_error("BBContainer not found for scope idx " +
                           std::to_string(id));
}

} // namespace IRI_STRUCTURAL
