#include "Storage/BindingsPool.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Storage/IRIContext.h"
#include "Storage/IridiumSEXP.h"
#include <cstdint>
#include <stdexcept>

namespace IRI_STORAGE {
using namespace IRI_STRUCTURAL;
void BindingsPool::allocateStub() {
  uint32_t storedIDX;
  // Store a empty stub in the ctx
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
    throw std::runtime_error(
        "Expected stub to be the first offset in the bindings ctx!!");
}

BindingMeta &BindingsPool::getMetaFromIRID(IRIContext &ctx, IRID bID) {
  IRI_TAG tag = IRI_NODE(ctx, bID).tag;
  if (tag == IRI_GEN::EnvBinding) {
    EnvBindingSEXP eb(bID, ctx);
    return get(eb.getLINK());
  } else if (tag == IRI_GEN::RemoteEnvBinding) {
    RemoteEnvBindingSEXP rb(bID, ctx);
    return get(rb.getLINK());
  } else if (tag == IRI_GEN::ScriptBinding) {
    ScriptBindingSEXP sb(bID, ctx);
    return get(sb.getLINK());
  } else if (tag == IRI_GEN::GlobalBinding) {
    GlobalBindingSEXP gb(bID, ctx);
    return get(gb.getLINK());
  } else {
    throw std::runtime_error("Failed to add to BindingsPool: " + dump_tag(tag));
  }
}
const BindingMeta &BindingsPool::getMetaFromIRID(IRIContext &ctx,
                                                 IRID bID) const {
  IRI_TAG tag = IRI_NODE(ctx, bID).tag;
  if (tag == IRI_GEN::EnvBinding) {
    EnvBindingSEXP eb(bID, ctx);
    return get(eb.getLINK());
  } else if (tag == IRI_GEN::RemoteEnvBinding) {
    RemoteEnvBindingSEXP rb(bID, ctx);
    return get(rb.getLINK());
  } else if (tag == IRI_GEN::ScriptBinding) {
    ScriptBindingSEXP sb(bID, ctx);
    return get(sb.getLINK());
  } else if (tag == IRI_GEN::GlobalBinding) {
    GlobalBindingSEXP gb(bID, ctx);
    return get(gb.getLINK());
  } else {
    throw std::runtime_error("Failed to add to BindingsPool: " + dump_tag(tag));
  }
}

BindingMeta &BindingsPool::allocate(IRIContext &ctx, IRID bID) {
  uint32_t storedIDX;
  IRI_TAG tag = IRI_NODE(ctx, bID).tag;

  // Store a BindingSEXP on the ctx
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
    EnvBindingSEXP eb(bID, ctx);
    eb.setLINK(storedIDX);
  } else if (tag == IRI_GEN::RemoteEnvBinding) {
    RemoteEnvBindingSEXP rb(bID, ctx);
    rb.setLINK(storedIDX);
  } else if (tag == IRI_GEN::ScriptBinding) {
    ScriptBindingSEXP sb(bID, ctx);
    sb.setLINK(storedIDX);
  } else if (tag == IRI_GEN::GlobalBinding) {
    GlobalBindingSEXP gb(bID, ctx);
    gb.setLINK(storedIDX);
  } else {
    throw std::runtime_error("Failed to add to BindingsPool: " + dump_tag(tag));
  }

  return get(storedIDX);
}

std::vector<IRID> BindingsPool::collectReferencedIrids() const {
  std::vector<IRID> ref_irids;

  for (size_t bPoolIndex = 1; bPoolIndex < bPool.size(); bPoolIndex++) {
    auto &meta = bPool[bPoolIndex];
    
    // IRID associated with BindingMeta
    ref_irids.push_back(meta.ID);
    
    // defs IRIDs in BindingMeta
    for (auto &d : meta.defs) {
      ref_irids.push_back(d);
    }

    // uses IRIDs in BindingMeta
    for (auto &u : meta.uses) {
      ref_irids.push_back(u);
    }
  }

  return ref_irids;
}

void BindingsPool::remapIrids(std::span<const IRID> old_to_new_irid_map) {
  auto remap = [&](IRID old_irid) -> IRID {
    if (old_irid >= old_to_new_irid_map.size() || old_to_new_irid_map[old_irid] == static_cast<IRID>(-1)) {
      throw std::runtime_error(
        "BindingsPool::remapIrids: garbage IRID in BindingMeta"
      );
    }
    return old_to_new_irid_map[old_irid];
  };

  // Skipping first bPool meta because allocateStub stores an empty stub at 0th index
  for (size_t bPoolIndex = 1; bPoolIndex < bPool.size(); bPoolIndex++) {
    auto &meta = bPool[bPoolIndex];

    // remap ID in BindingMeta
    meta.ID = remap(meta.ID);

    // remap defs IRIDs in BindingMeta
    for (auto &d : meta.defs) {
      d = remap(d);
    }

    // remap uses IRIDs in BindingMeta
    for (auto &u : meta.uses) {
      u = remap(u);
    }
  }
}

static void printEnvBindingDetails(IRIContext &ctx, std::ostream &oss,
                                   IRID ID) {
  EnvBindingSEXP eb(ID, ctx);
  oss << "["
      << "REF: " << eb.getREFIDX() << ", SCOPE: " << eb.getSCOPE()
      << ", NEXT: " << eb.getNEXT() << ", LINK: " << eb.getLINK() << "]";
}

static void printRemoteEnvBindingDetails(IRIContext &ctx, std::ostream &oss,
                                         IRID ID) {
  RemoteEnvBindingSEXP rb(ID, ctx);

  oss << "["
      << "REF: " << rb.getREFIDX() << ", LINK: " << rb.getLINK();
  oss << (rb.hasMODULE() ? ",module" : "");
  oss << (rb.hasMODULEI() ? ",modulei" : "");
  oss << (rb.hasMODULENSI() ? ",modulensi" : "");
  oss << "]";
  IRID next = rb.getArg_ParentReference();
  auto nextTAG = IRI_NODE(ctx, next).tag;
  if (nextTAG == IRI_GEN::EnvBinding) {
    oss << "->";
    printEnvBindingDetails(ctx, oss, next);
  } else if (nextTAG == IRI_GEN::RemoteEnvBinding) {
    oss << "-";
    printRemoteEnvBindingDetails(ctx, oss, next);
  } else {
    throw std::runtime_error("RemoteEnvBinding wrapped incorrectly");
  }
}

void BindingMeta::dump(IRIContext &ctx, std::ostream &oss, bool full) const {
  IRI_TAG tag = IRI_NODE(ctx, ID).tag;
  // oss << currOffset << "@";
  if (tag == IRI_GEN::EnvBinding) {
    EnvBindingSEXP eb(ID, ctx);
    oss << "{" << ctx.storage.strings.get(eb.getNAME());
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
      printEnvBindingDetails(ctx, oss, ID);
    }
  } else if (tag == IRI_GEN::RemoteEnvBinding) {
    RemoteEnvBindingSEXP rb(ID, ctx);
    EnvBindingSEXP eb(IRI_HELPERS::resolveRemoteBinding(ctx, ID), ctx);
    oss << "{^" << ctx.storage.strings.get(eb.getNAME()) << "}";
    if (full) {
      oss << "-";
      printRemoteEnvBindingDetails(ctx, oss, ID);
    }
  } else if (tag == IRI_GEN::ScriptBinding) {
    ScriptBindingSEXP sb(ID, ctx);
    oss << "{*" << ctx.storage.strings.get(sb.getNAME());
    oss << ":";
    oss << (sb.hasJSLET() ? "let" : "");
    oss << (sb.hasJSCONST() ? "const" : "");
    oss << (sb.hasJSVAR() ? "var" : "");
    oss << "}";
  } else if (tag == IRI_GEN::GlobalBinding) {
    GlobalBindingSEXP gb(ID, ctx);
    oss << "{" << ctx.storage.strings.get(gb.getNAME()) << "}";
  } else {
    throw std::runtime_error("Invalid BindingMeta");
  }
}
} // namespace IRI_STORAGE
