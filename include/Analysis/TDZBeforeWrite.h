#pragma once

#include "Analysis/PassManager.h"
#include "Analysis/TDZA.h"
#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Support/PP/PPDispatch.hpp"
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace IRI_STRUCTURAL {

//
// I am removing SAFE flag (basically TDZ check is needed or not) from Write
// Nodes. The main gripe I have with this flag is that write become messy, every
// analysis pass need to worry about them, and what not. Mostly TDZ check node
// was introduced a while back to make this flag redundant but I was never able
// to properly get rid of it before. This pass is a neat idea, instead of doing
// anything interesting, it basically asserts that a binding being written to is
// always SAFE, this property must trivially hold if the TDZ emission is right.
//
// ~ Meetesh
//
struct TDZBeforeWritePass {
  // A binding's own declaration store initializes it, so it is not a write
  // into a zone and needs no guard.
  IRI_CORE_PASSES::DECLSTORES &declStores;

  TDZBeforeWritePass(IRI_CORE_PASSES::DECLSTORES &declStores)
      : declStores(declStores) {}

  // Does this node read the binding? Bindings are singletons within a closure,
  // so a read and a write of the same binding name the same IRID.
  static bool readsBinding(IRIContext &ctx, IRID node, IRID binding) {
    if (IRI_NODE(ctx, node).tag == IRI_GEN::EnvRead &&
        ctx.storage.nodes.get_args_view(node)[0] == binding)
      return true;
    for (IRID child : ctx.storage.nodes.get_args_view(node))
      if (readsBinding(ctx, child, binding))
        return true;
    return false;
  }

  // The binding a write targets, or NO_IRID when there is no zone to guard:
  // the once-only 'this' store, which checks itself, or a store to a global or
  // script binding.
  static IRID checkedWrite(IRIContext &ctx, IRID write) {
    IRID target;
    switch (IRI_NODE(ctx, write).tag) {
    case IRI_GEN::LWrite: {
      IRI_GEN::LWriteSEXP w(write, ctx);
      if (w.hasTHISINIT())
        return NO_IRID;
      target = w.getArg_LValTarget();
      break;
    }
    case IRI_GEN::RWrite: {
      IRI_GEN::RWriteSEXP w(write, ctx);
      if (w.hasTHISINIT())
        return NO_IRID;
      target = w.getArg_LValTarget();
      break;
    }
    case IRI_GEN::MWrite:
      target = IRI_GEN::MWriteSEXP(write, ctx).getArg_LValTarget();
      break;
    default:
      return NO_IRID;
    }
    return trackedBinding(ctx, target) ? target : NO_IRID;
  }

  static std::string bindingName(IRIContext &ctx, IRID binding) {
    if (IRI_NODE(ctx, binding).tag == IRI_GEN::RemoteEnvBinding)
      binding = IRI_HELPERS::resolveRemoteBinding(ctx, binding);
    if (IRI_NODE(ctx, binding).tag != IRI_GEN::EnvBinding)
      return IRI_GEN::dump_tag(IRI_NODE(ctx, binding).tag);
    return std::string(ctx.storage.strings.get(
        IRI_GEN::EnvBindingSEXP(binding, ctx).getNAME()));
  }

  bool run(IRICFG &cfg, AnalysisManager &am) {
    auto &ctx = cfg.ctx;
    const TDZAnalysisResult &tdz = am.getResult<TDZAnalysis>(cfg);

    for (BBIDX h = 0; h < cfg.numBBs(); h++) {
      const std::vector<IRID> &stmts = cfg.at(h).stmts;
      for (size_t i = 0; i < stmts.size(); i++) {
        IRID write = stmts[i];
        IRID binding = checkedWrite(ctx, write);
        if (binding == NO_IRID || declStores.count(write))
          continue;

        // We have corner cases not yet handled, a++, read in the RVAL
        // helps us here... Will remove this soon too
        if (readsBinding(ctx, write, binding) ||
            tdz.getStateBeforeStatement(h, i).getLattice(binding).kind !=
                TDZLattice::TDZ)
          continue;

        std::string block;
        for (size_t j = 0; j < stmts.size(); j++)
          block += "\n    [" + std::to_string(j) + "] " +
                   ppStatement(PPContext(ctx), stmts[j]);
        throw std::runtime_error(std::string("TDZBeforeWrite: write to '") +
                                 bindingName(ctx, binding) + "' in BB" +
                                 std::to_string(h) +
                                 " has no TDZ read before it:" + block);
      }
    }
    return false;
  }
};

} // namespace IRI_STRUCTURAL
