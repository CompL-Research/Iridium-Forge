#pragma once
#include "Config.h"
#include "Generated/IridiumEnums.h"
#include <algorithm> // for std::find
#include <cassert>   // for debug safety
#include <cstdint>
#include <small/vector.hpp>

namespace IRI_STORAGE {
using namespace IRI_GEN;

class IridiumPool;

struct BindingMeta {
  IRID ID = 0;
  IRI_TAG tag = IRI_TAG::Null;
  uint32_t backOffset = 0;

  // SBO (Small Buffer Optimization) sizes tuned for your 3-def / N-use profile
  small::vector<IRID, 3> defs;
  small::vector<uint32_t, 4> uses;
  small::vector<uint32_t, 1> forwardOffsets;

  void reset() {
    ID = 0;
    tag = IRI_TAG::Null;
    backOffset = 0;
    defs.clear();
    uses.clear();
    forwardOffsets.clear();
  }
};

class BindingsPool {
public:
  static uint32_t getBPoolOffsetForEnvBinding(IridiumPool &pool, IRID id);

  explicit BindingsPool(size_t size) { bPool.reserve(size); }

  // --- Array Access Overloads ---

  // Non-const access: pool[idx]
  BindingMeta &operator[](uint32_t idx) {
    assert(idx < bPool.size() && "BindingPool index out of bounds");
    return bPool[idx];
  }

  // Const access: pool[idx] (used for read-only analysis)
  const BindingMeta &operator[](uint32_t idx) const {
    assert(idx < bPool.size() && "BindingPool index out of bounds");
    return bPool[idx];
  }

  // --- Pool Management ---

  void allocateStub();

  void allocate(IridiumPool &pool, IRID id);

  // --- Relationship Helpers ---

  void addDef(uint32_t bindingIdx, IRID statementID) {
    (*this)[bindingIdx].defs.push_back(statementID);
  }

  void addUse(uint32_t bindingIdx, uint32_t statementID) {
    (*this)[bindingIdx].uses.push_back(statementID);
  }

  void addCapture(uint32_t parentIdx, uint32_t childRemoteIdx) {
    auto &parent = (*this)[parentIdx];
    auto &child = (*this)[childRemoteIdx];

    parent.forwardOffsets.push_back(childRemoteIdx);
    child.backOffset = parentIdx;
  }

  void removeUse(uint32_t bindingIdx, uint32_t statementID) {
    auto &meta = (*this)[bindingIdx];

    // Efficient Swap-and-Pop
    auto it = std::find(meta.uses.begin(), meta.uses.end(), statementID);
    if (it != meta.uses.end()) {
      *it = meta.uses.back();
      meta.uses.pop_back();
    }
  }

  void removeDef(uint32_t bindingIdx, uint32_t statementID) {
    auto &meta = (*this)[bindingIdx];

    // Efficient Swap-and-Pop
    auto it = std::find(meta.defs.begin(), meta.defs.end(), statementID);
    if (it != meta.defs.end()) {
      *it = meta.defs.back();
      meta.defs.pop_back();
    }
  }

  void dump(std::ostream &oss) const {
    oss << "=== BindingsPool Dump ===\n";
    oss << "Pool Size: " << bPool.size() << "\n";

    for (size_t i = 0; i < bPool.size(); ++i) {
      const auto &meta = bPool[i];

      oss << "  [" << i << "] ID: " << meta.ID
          << ", Tag: " << dump_tag(meta.tag)
          << ", BackOffset: " << meta.backOffset << "\n";

      // Dump Defs
      oss << "      Defs: [";
      for (size_t j = 0; j < meta.defs.size(); ++j) {
        oss << meta.defs[j] << (j + 1 < meta.defs.size() ? ", " : "");
      }
      oss << "]\n";

      // Dump Uses
      oss << "      Uses: [";
      for (size_t j = 0; j < meta.uses.size(); ++j) {
        oss << meta.uses[j] << (j + 1 < meta.uses.size() ? ", " : "");
      }
      oss << "]\n";

      // Dump Forward Offsets
      oss << "      ForwardOffsets: [";
      for (size_t j = 0; j < meta.forwardOffsets.size(); ++j) {
        oss << meta.forwardOffsets[j]
            << (j + 1 < meta.forwardOffsets.size() ? ", " : "");
      }
      oss << "]\n";
    }

    // Dump FreeList
    oss << "FreeList Size: " << freeList.size() << " [";
    for (size_t i = 0; i < freeList.size(); ++i) {
      oss << freeList[i] << (i + 1 < freeList.size() ? ", " : "");
    }
    oss << "]\n";
    oss << "=========================\n";
  }

private:
  void deallocate(uint32_t index) {
    bPool[index].reset();
    freeList.push_back(index);
  }

  //
  // TODO: I will work on pruning later... we dont want the IRIS map to go out
  // of sync due to this
  //
  // // Cascade pruning if this was the last use
  // if (shouldPrune(bindingIdx)) {
  //   triggerPrune(bindingIdx);
  // }

  // bool shouldPrune(uint32_t idx) const {
  //   const auto &m = bPool[idx];
  //   // Only prune Remote Bindings that have zero activity
  //   return m.tag == IRI_TAG::RemoteEnvBinding && m.uses.empty() &&
  //          m.defs.empty() && m.forwardOffsets.empty();
  // }

  // void triggerPrune(uint32_t idx) {
  //   uint32_t parentIdx = bPool[idx].backOffset;
  //   deallocate(idx);

  //   // Remove the child from parent's forward tracking
  //   auto &pForward = bPool[parentIdx].forwardOffsets;
  //   auto it = std::find(pForward.begin(), pForward.end(), idx);
  //   if (it != pForward.end()) {
  //     *it = pForward.back();
  //     pForward.pop_back();

  //     // Recursively prune up the chain
  //     if (shouldPrune(parentIdx)) {
  //       triggerPrune(parentIdx);
  //     }
  //   }
  // }

  small::vector<BindingMeta, 0> bPool;
  small::vector<uint32_t, 0> freeList;
};

} // namespace IRI_STORAGE
