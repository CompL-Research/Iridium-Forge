#include "Storage/BindingsPool.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Storage/IridiumPool.h"
#include "Storage/IridiumSEXP.h"
#include <cstdint>
#include <stdexcept>

namespace IRI_STORAGE {
using namespace IRI_STRUCTURAL;
void BindingsPool::allocateStub() {
  uint32_t storedIDX;
  // Store a empty stub in the pool
  // Ensure that this always occupies the 0th index
  if (!freeList.empty()) {
    uint32_t index = freeList.back();
    freeList.pop_back();
    bPool[index].ID = 0;
    storedIDX = index;
  } else {
    uint32_t newIdx = static_cast<uint32_t>(bPool.size());
    bPool.emplace_back();
    bPool[newIdx].ID = 0;
    storedIDX = newIdx;
  }

  if (storedIDX != 0)
    throw std::runtime_error("Expected stub to be the first offset in the bindings pool!!");
}

BindingMeta & BindingsPool::getMetaFromIRID(IridiumPool &pool, IRID bID) {
  IRI_TAG tag = pool[bID].tag;
  if (tag == IRI_GEN::EnvBinding) {
    EnvBindingSEXP eb(bID, pool);
    return (*this)[eb.getLINK()];
  } else if (tag == IRI_GEN::RemoteEnvBinding) {
    RemoteEnvBindingSEXP rb(bID, pool);
    return (*this)[rb.getLINK()];
  } else if (tag == IRI_GEN::ScriptBinding) {
    ScriptBindingSEXP sb(bID, pool);
    return (*this)[sb.getLINK()];
  } else if (tag == IRI_GEN::GlobalBinding) {
    GlobalBindingSEXP gb(bID, pool);
    return (*this)[gb.getLINK()];
  } else {
    throw std::runtime_error("Failed to add to BindingsPool: " + dump_tag(tag));
  }
}
const BindingMeta & BindingsPool::getMetaFromIRID(IridiumPool &pool, IRID bID) const {
  IRI_TAG tag = pool[bID].tag;
  if (tag == IRI_GEN::EnvBinding) {
    EnvBindingSEXP eb(bID, pool);
    return (*this)[eb.getLINK()];
  } else if (tag == IRI_GEN::RemoteEnvBinding) {
    RemoteEnvBindingSEXP rb(bID, pool);
    return (*this)[rb.getLINK()];
  } else if (tag == IRI_GEN::ScriptBinding) {
    ScriptBindingSEXP sb(bID, pool);
    return (*this)[sb.getLINK()];
  } else if (tag == IRI_GEN::GlobalBinding) {
    GlobalBindingSEXP gb(bID, pool);
    return (*this)[gb.getLINK()];
  } else {
    throw std::runtime_error("Failed to add to BindingsPool: " + dump_tag(tag));
  }
}

BindingMeta & BindingsPool::allocate(IridiumPool &pool, IRID bID) {
  uint32_t storedIDX;
  IRI_TAG tag = pool[bID].tag;

  // Store a BindingSEXP on the pool
  if (!freeList.empty()) {
    uint32_t index = freeList.back();
    freeList.pop_back();
    bPool[index].currOffset = index;
    bPool[index].ID = bID;
    bPool[index].isARGX = false;
    bPool[index].isIMPLICITOVERRIDEABLE = false;
    bPool[index].tombstone = false;
    bPool[index].backOffset = 0; // point to the stub
    storedIDX = index;
  } else {
    uint32_t newIdx = static_cast<uint32_t>(bPool.size());
    bPool.emplace_back();
    bPool[newIdx].currOffset = newIdx;
    bPool[newIdx].ID = bID;
    bPool[newIdx].isARGX = false;
    bPool[newIdx].isIMPLICITOVERRIDEABLE = false;
    bPool[newIdx].tombstone = false;
    bPool[newIdx].backOffset = 0; // point to the stub
    storedIDX = newIdx;
  }

  // Create links inside the object
  if (tag == IRI_GEN::EnvBinding) {
    EnvBindingSEXP eb(bID, pool);
    eb.setLINK(storedIDX);
  } else if (tag == IRI_GEN::RemoteEnvBinding) {
    RemoteEnvBindingSEXP rb(bID, pool);
    rb.setLINK(storedIDX);
  } else if (tag == IRI_GEN::ScriptBinding) {
    ScriptBindingSEXP sb(bID, pool);
    sb.setLINK(storedIDX);
  } else if (tag == IRI_GEN::GlobalBinding) {
    GlobalBindingSEXP gb(bID, pool);
    gb.setLINK(storedIDX);
  } else {
    throw std::runtime_error("Failed to add to BindingsPool: " + dump_tag(tag));
  }

  return (*this)[storedIDX];
}

static void printEnvBindingDetails(IridiumPool &pool, std::ostream &oss, IRID ID) {
  EnvBindingSEXP eb(ID, pool);
  oss << "["
      << "REF: " << eb.getREFIDX()
      << ", SCOPE: " << eb.getSCOPE()
      << ", NEXT: " << eb.getNEXT()
      << ", LINK: " << eb.getLINK()
      << "]";
}

static void printRemoteEnvBindingDetails(IridiumPool &pool, std::ostream &oss, IRID ID) {
  RemoteEnvBindingSEXP rb(ID, pool);

  oss << "["
      << "REF: " << rb.getREFIDX()
      << ", LINK: " << rb.getLINK();
  oss << (rb.hasMODULE() ? ",module" : "");
  oss << (rb.hasMODULEI() ? ",modulei" : "");
  oss << (rb.hasMODULENSI() ? ",modulensi" : "");
  oss << "]";
  IRID next = rb.getArg_ParentReference();
  auto nextTAG = pool[next].tag;
  if (nextTAG == IRI_GEN::EnvBinding) {
    oss << "->";
    printEnvBindingDetails(pool, oss, next);
  } else if (nextTAG == IRI_GEN::RemoteEnvBinding) {
    oss << "-";
    printRemoteEnvBindingDetails(pool, oss, next);
  } else {
    throw std::runtime_error("RemoteEnvBinding wrapped incorrectly");
  }
}

void BindingMeta::dump(IridiumPool &pool, std::ostream &oss, bool full) const {
  IRI_TAG tag = pool[ID].tag;
  // oss << currOffset << "@";
  if (tag == IRI_GEN::EnvBinding) {
    EnvBindingSEXP eb(ID, pool);
    oss << "{" << pool.strings.get(eb.getNAME());
    oss << ":";
    oss << (eb.hasJSARG() ? "arg" : "");
    oss << (eb.hasJSRESTARG() ? "*arg" : "");
    oss << (eb.hasJSLET() ? "let" : "");
    oss << (eb.hasJSCONST() ? "const" : "");
    oss << (eb.hasJSVAR() ? "var" : "");
    oss << (isCaptured() ? ":C" : "");
    oss << "}";
    if (full) {
      oss << "->";
      printEnvBindingDetails(pool, oss, ID);
    }
  } else if (tag == IRI_GEN::RemoteEnvBinding) {
    RemoteEnvBindingSEXP rb(ID, pool);
    EnvBindingSEXP eb(IRI_HELPERS::resolveRemoteBinding(pool, ID), pool);
    oss << "{^" << pool.strings.get(eb.getNAME()) << "}";
    if (full) {
      oss << "-";
      printRemoteEnvBindingDetails(pool, oss, ID);
    }
  } else if (tag == IRI_GEN::ScriptBinding) {
    ScriptBindingSEXP sb(ID, pool);
    oss << "{*" << pool.strings.get(sb.getNAME());
    oss << ":";
    oss << (sb.hasJSLET() ? "let" : "");
    oss << (sb.hasJSCONST() ? "const" : "");
    oss << (sb.hasJSVAR() ? "var" : "");
    oss << "}";
  } else if (tag == IRI_GEN::GlobalBinding) {
    GlobalBindingSEXP gb(ID, pool);
    oss << "{" << pool.strings.get(gb.getNAME()) << "}";
  } else {
    throw std::runtime_error("Invalid BindingMeta");
  }
}
} // namespace IRI_STORAGE
