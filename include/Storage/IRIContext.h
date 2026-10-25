#pragma once
#include "Generated/IridiumPassFlags.h"
#include "IRIDebug.h"
#include "IRIStorage.h"
#include <memory>
#include <unordered_map>

namespace IRI_STRUCTURAL {
class IRIS;
class ClosureTree;
} // namespace IRI_STRUCTURAL

namespace IRI_STORAGE {

class IRIContext {
public:
  IRIStorage storage;
  IRIDebug debugger;
  IRI_GEN::PassFlags flags;

  double lastBBIDX = 0;
  std::shared_ptr<IRI_STRUCTURAL::IRIS> iris = nullptr;
  std::shared_ptr<IRI_STRUCTURAL::ClosureTree> closureTree = nullptr;

  IRIContext() = default;
  IRIContext(const IRIContext &) = delete;
  IRIContext &operator=(const IRIContext &) = delete;
};

} // namespace IRI_STORAGE

#define IRI_NODE(ctx, id) ((ctx).storage.nodes.get_node(id))
