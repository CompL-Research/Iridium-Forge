#pragma once
#include "Config.h"
#include "Generated/IridiumEnums.h"
#include <cassert>   // for debug safety
#include <cstdint>
#include <small/vector.hpp>

namespace IRI_STORAGE {
using namespace IRI_GEN;

class IridiumPool;

struct BindingMeta {
  uint32_t currOffset = 0;
  IRID ID = 0;
  bool isARGX = false;
  bool isIMPLICITOVERRIDEABLE = false;
  bool tombstone = false;
  bool evalTainted = false;
  uint32_t backOffset = 0;

  // SBO (Small Buffer Optimization) sizes tuned for your 3-def / N-use profile
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

  static void connect(BindingMeta & child, BindingMeta & parent) {
    child.backOffset = parent.currOffset;
    parent.forwardOffsets.push_back(child.currOffset);
  }

  void addDef(IRID);
  void addUse(IRID);

  bool isCaptured() const {
    return forwardOffsets.size() > 0;
  }

  void markEvalTainted() {
    evalTainted = true;
  }

  bool isEvalTainted() const {
    return evalTainted;
  }

  bool isDead() {
    return uses.size() == 0 && forwardOffsets.size() == 0;
  }

  void dump(IridiumPool &, std::ostream &, bool full = true) const;
};

class BindingsPool {
public:
  explicit BindingsPool(size_t size) { bPool.reserve(size); allocateStub(); }
  explicit BindingsPool() { allocateStub(); }

  // --- Array Access Overloads ---
  BindingMeta &operator[](uint32_t idx) {
    assert(idx < bPool.size() && "BindingPool index out of bounds");
    return bPool[idx];
  }

  const BindingMeta &operator[](uint32_t idx) const {
    assert(idx < bPool.size() && "BindingPool index out of bounds");
    return bPool[idx];
  }

  BindingMeta &getMetaFromIRID(IridiumPool &, IRID);
  const BindingMeta &getMetaFromIRID(IridiumPool &, IRID) const;

  // --- Pool Management ---
  BindingMeta & allocate(IridiumPool &pool, IRID id);

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
