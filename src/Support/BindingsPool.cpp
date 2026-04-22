#include "Storage/BindingsPool.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Storage/IridiumPool.h"
#include <cstdint>
#include <stdexcept>

namespace IRI_STORAGE {

uint32_t BindingsPool::getBPoolOffsetForEnvBinding(IridiumPool &pool, IRID bID) {
  IRI_TAG tag;
  if (pool[bID].tag == IRI_GEN::EnvBinding) {
    EnvBindingSEXP b(bID, pool);
    assert(b.hasLINK() && "Tried to get LINK on EnvBinding whose LINK is not set");
    return std::get<uint32_t>(b.get_flag(EnvBindingSEXP::FLAG_IDX_LINK));
  } else if (pool[bID].tag == IRI_GEN::RemoteEnvBinding) {
    RemoteEnvBindingSEXP rb(bID, pool);
    assert(rb.hasLINK() && "Tried to get LINK on RemoteEnvBinding whose LINK is not set");
    return std::get<uint32_t>(rb.get_flag(RemoteEnvBindingSEXP::FLAG_IDX_LINK));
  } else {
    throw std::runtime_error("Failed to get bindings pool offset, invalid tag");
  }
}

void BindingsPool::allocateStub() {
  uint32_t storedIDX;
  // Store a empty stub in the pool
  // Ensure that this always occupies the 0th index
  if (!freeList.empty()) {
    uint32_t index = freeList.back();
    freeList.pop_back();
    bPool[index].ID = 0;
    bPool[index].tag = IRI_GEN::NOP;
    storedIDX = index;
  } else {
    uint32_t newIdx = static_cast<uint32_t>(bPool.size());
    bPool.emplace_back();
    bPool[newIdx].ID = 0;
    bPool[newIdx].tag = IRI_GEN::NOP;
    storedIDX = newIdx;
  }

  if (storedIDX != 0)
    throw std::runtime_error("Expected stub to be the first offset in the bindings pool!!");
}

void BindingsPool::allocate(IridiumPool &pool, IRID bID) {
  IRI_TAG tag;
  if (pool[bID].tag == IRI_GEN::EnvBinding) {
    tag = EnvBinding;
  } else if (pool[bID].tag == IRI_GEN::RemoteEnvBinding) {
    tag = RemoteEnvBinding;
  } else {
    throw std::runtime_error("Expected bindings to be stored in the bindings pool");
  }
  uint32_t storedIDX;

  // Store a BindingSEXP on the pool
  if (!freeList.empty()) {
    uint32_t index = freeList.back();
    freeList.pop_back();
    bPool[index].ID = bID;
    bPool[index].tag = tag;
    bPool[index].backOffset = 0; // point to the stub
    storedIDX = index;
  } else {
    uint32_t newIdx = static_cast<uint32_t>(bPool.size());
    bPool.emplace_back();
    bPool[newIdx].ID = bID;
    bPool[newIdx].tag = tag;
    bPool[newIdx].backOffset = 0; // point to the stub
    storedIDX = newIdx;
  }

  // Create links inside the object
  if (pool[bID].tag == IRI_GEN::EnvBinding) {
    EnvBindingSEXP eBindingSEXP(bID, pool);
    eBindingSEXP.mutate_flag(EnvBindingSEXP::FLAG_IDX_LINK) = storedIDX;
  } else if (pool[bID].tag == IRI_GEN::RemoteEnvBinding) {

    RemoteEnvBindingSEXP reBindingSEXP(bID, pool);
    reBindingSEXP.mutate_flag(RemoteEnvBindingSEXP::FLAG_IDX_LINK) = storedIDX;
  }

}
} // namespace IRI_STORAGE
