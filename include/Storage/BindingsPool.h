#pragma once
#include "Config.h"
#include "Generated/IridiumEnums.h"
#include "PoolMemStats.h"
#include <cassert> // for debug safety
#include <cstdint>
#include <small/vector.hpp>
#include <span>

namespace IRI_STORAGE {
using namespace IRI_GEN;

class IRIContext;

struct BindingMeta {
  uint32_t currOffset = 0;
  IRID ID = 0;
  bool isARGX = false;
  bool isIMPLICITOVERRIDEABLE = false;
  bool tombstone = false;
  bool evalTainted = false;
  uint32_t backOffset = 0;

  small::vector<IRID, 3> defs;
  small::vector<IRID, 4> uses;
  small::vector<uint32_t, 1> forwardOffsets;

  void reset() {
    currOffset = 0;
    ID = 0;
    isARGX = false;
    backOffset = 0;
    defs.clear();
    uses.clear();
    forwardOffsets.clear();
  }

  static void connect(BindingMeta &child, BindingMeta &parent) {
    child.backOffset = parent.currOffset;
    parent.forwardOffsets.push_back(child.currOffset);
  }

  void addDef(IRID);
  void addUse(IRID);

  bool isCaptured() const { return forwardOffsets.size() > 0; }

  void markEvalTainted() { evalTainted = true; }

  bool isEvalTainted() const { return evalTainted; }

  bool isDead() { return uses.size() == 0 && forwardOffsets.size() == 0; }

  void dump(IRIContext &, std::ostream &, bool full = true) const;
};

class BindingsPool {
public:
  explicit BindingsPool(size_t size) {
    bPool.reserve(size);
    allocateStub();
  }
  explicit BindingsPool() { allocateStub(); }

  BindingMeta &get(uint32_t idx) {
    assert(idx < bPool.size() && "BindingPool index out of bounds");
    return bPool[idx];
  }

  const BindingMeta &get(uint32_t idx) const {
    assert(idx < bPool.size() && "BindingPool index out of bounds");
    return bPool[idx];
  }

  BindingMeta &getMetaFromIRID(IRIContext &, IRID);
  const BindingMeta &getMetaFromIRID(IRIContext &, IRID) const;

  BindingMeta &allocate(IRIContext &ctx, IRID id);

  PoolMemStats telemetry() const {
    return {bPool.size(), bPool.size() * sizeof(BindingMeta),
            bPool.capacity() * sizeof(BindingMeta)};
  }

  // IRIS can declare bindings during core passes
  // which are not connected by args of any node until iris commit
  // Add those reference IRIDs into the root set to include in reachability result
  // so that bindingsPool->remapIrids don't see these IRIDs as garbage
  std::vector<IRID> collectReferencedIrids() const;

  // bPool holds BindingMeta per binding
  // Remap IRIDs referenced by BindingMeta after compaction
  // ID, defs, uses -> only these are IRIDs of IridiumPool
  void remapIrids(std::span<const IRID> old_to_new_irid_map);

private:
  void deallocate(uint32_t index) {
    bPool[index].reset();
    freeList.push_back(index);
  }
  void allocateStub();

  small::vector<BindingMeta, 0> bPool;
  small::vector<uint32_t, 0> freeList;
};

} // namespace IRI_STORAGE

#define IRI_BINDING_AT(bindingsPool, idx) ((bindingsPool)->get(idx))
