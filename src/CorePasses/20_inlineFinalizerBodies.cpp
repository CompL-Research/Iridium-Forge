#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIClone.hpp"
#include "Support/IRIS.hpp"
#include <algorithm>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

// An InvokeFinalizer statement and the scope of the body it invokes.
struct IVSite {
  IRID stmt;
  double F;
  double entryScope;
};

void _20_IFB(IRIContext &ctx, IRID fileSEXP,
             IRI_CONTEXTMAP &iridiumBuildContext, CTARGETS &continueTargets) {
  FileSupport file(fileSEXP, ctx);

  // Freeze dispatch targets: materialize the scope table onto every BB.
  // Last valid scope lookup; after this ExceptionIDX is the only source.
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    double closureScope = bbc.getScopeIDX();
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);
      ctx.iris->declareExceptionIDX(
          closureScope, bb.getIDX(),
          ctx.iris->getExceptionTargetForScope(bb.getScopeIDX()));
    }
  }

  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    IRID bbList = bbc.getArg_BB();

    std::vector<IVSite> sites;
    for (auto [bbID, _] : bbc.bbs())
      for (auto s : ctx.storage.nodes.get_args(bbID))
        if (IRI_NODE(ctx, s).tag == IRI_TAG::InvokeFinalizer)
          sites.push_back(
              {s, InvokeFinalizerSEXP(s, ctx).getIDX(),
               BBSupport(bbc.getBBByIDX(InvokeFinalizerSEXP(s, ctx).getIDX()),
                         ctx)
                   .getScopeIDX()});

    // Structured regions are single-entry. Templates only: cloning shares
    // the scope, so each expansion adds an entry of its own.
    for (size_t i = 0; i < sites.size(); i++) {
      bool seen = false;
      for (size_t k = 0; k < i; k++)
        if (sites[k].F == sites[i].F)
          seen = true;
      if (seen)
        continue;
      for (auto [bbID, _] : bbc.bbs()) {
        BBSupport supp(bbID, ctx);
        if (supp.getScopeIDX() != sites[i].entryScope ||
            supp.getIDX() == sites[i].F)
          continue;
        for (auto [srcID, _] : bbc.bbs()) {
          BBSupport srcSupp(srcID, ctx);
          if (ctx.iris->hasScopePath(srcSupp.getScopeIDX(),
                                     sites[i].entryScope))
            continue; // descendant re-entry is not an entry
          for (double t : bbTargets(ctx, srcID))
            if (t == supp.getIDX())
              throw std::runtime_error(
                  "_20_IFB: finalizer region has two entry blocks: F=" +
                  std::to_string((int)sites[i].F) + " entry2=" +
                  std::to_string((int)supp.getIDX()) + " from BB " +
                  std::to_string((int)srcSupp.getIDX()));
        }
      }
    }

    // Innermost first: a clone duplicates static control flow only when
    // its nested finalizers are already expanded (outer rides on inner).
    while (!sites.empty()) {
      size_t pick = 0;
      for (size_t i = 0; i < sites.size(); i++) {
        bool hasNested = false;
        for (size_t j = 0; j < sites.size(); j++)
          if (i != j && sites[j].entryScope != sites[i].entryScope &&
              ctx.iris->hasScopePath(sites[j].entryScope, sites[i].entryScope))
            hasNested = true;
        if (!hasNested) {
          pick = i;
          break;
        }
      }
      IVSite site = sites[pick];
      sites.erase(sites.begin() + pick);

      // A previous site may have split this statement into another BB.
      IRID siteBB = 0;
      for (auto [bbID, _] : bbc.bbs()) {
        auto a = ctx.storage.nodes.get_args(bbID);
        if (std::find(a.begin(), a.end(), site.stmt) != a.end()) {
          siteBB = bbID;
          break;
        }
      }
      if (siteBB == 0)
        throw std::runtime_error("_20_IFB: InvokeFinalizer statement lost");

      double F = site.F;

      // Complete clone of the body region; the original stays a template.
      RegionClone rc = cloneRegion(ctx, bbc, F);

      std::vector<double> order;
      for (auto &[d, copy] : rc.cloner.targetMap)
        order.push_back(d);
      std::sort(order.begin(), order.end()); // deterministic fixups/dumps

      // Innermost-first invariant: nested finalizers are already gone.
      for (double d : order)
        for (auto s : ctx.storage.nodes.get_args(bbc.getBBByIDX(d)))
          if (IRI_NODE(ctx, s).tag == IRI_TAG::InvokeFinalizer)
            throw std::runtime_error(
                "_20_IFB: InvokeFinalizer inside a cloned region: F=" +
                std::to_string((int)F) + " nestedF=" +
                std::to_string((int)InvokeFinalizerSEXP(s, ctx).getIDX()));

      for (double d : order) {
        IRID src = bbc.getBBByIDX(d);
        IRID nb = bbc.getBBByIDX(rc.cloner.targetMap[d]);

        // Copies escape with plain jumps: no return address to drop.
        auto st = ctx.storage.nodes.get_args(nb);
        st.erase(std::remove_if(st.begin(), st.end(),
                                [&](IRID s) {
                                  return IRI_NODE(ctx, s).tag ==
                                         IRI_TAG::ExitFinalizer;
                                }),
                 st.end());
        ctx.storage.nodes.set_args(nb, st);

        // Carry continue-target side-table entries (consumed by _22).
        for (auto s : ctx.storage.nodes.get_args(src)) {
          if (IRI_NODE(ctx, s).tag != IRI_TAG::Goto)
            continue;
          auto it = continueTargets.find(s);
          if (it == continueTargets.end())
            continue;
          auto c = rc.cloner.nodeMap.find(s);
          if (c != rc.cloner.nodeMap.end())
            continueTargets[c->second] = it->second;
        }
      }

      // IV becomes a jump into the copy; its tail becomes the continuation
      // block where the copy's completion lands.
      auto siteArgs = ctx.storage.nodes.get_args(siteBB);
      size_t pos = std::find(siteArgs.begin(), siteArgs.end(), site.stmt) -
                   siteArgs.begin();
      if (pos + 1 >= siteArgs.size())
        throw std::runtime_error("_20_IFB: InvokeFinalizer is terminal");
      IRID contBB = splitBBSuffix(ctx, siteBB, pos + 1, bbc.getScopeIDX());
      double contIDX = BBSupport(contBB, ctx).getIDX();
      std::vector<IRID> list = ctx.storage.nodes.get_args(bbList);
      list.push_back(contBB);
      ctx.storage.nodes.set_args(bbList, list);

      ctx.storage.nodes.update_arg_inplace(
          siteBB, pos, GotoSEXP::create(ctx, false, rc.entryIDX));

      // A completing body's ret copy jumps to the continuation; bodies
      // that always throw/returns have no ret copy (no completion edge).
      auto rit = rc.cloner.targetMap.find(ctx.iris->getFinalizerRetBBIDX(F));
      if (rit != rc.cloner.targetMap.end()) {
        IRID cloneRetBB = bbc.getBBByIDX(rit->second);
        auto retArgs = ctx.storage.nodes.get_args(cloneRetBB);
        if (IRI_NODE(ctx, retArgs.back()).tag == IRI_TAG::Ret)
          ctx.storage.nodes.update_arg_inplace(
              cloneRetBB, retArgs.size() - 1,
              GotoSEXP::create(ctx, false, contIDX));
      }
    }
  }

  // Ret tails live only in dead original regions; poison them so a stale
  // reference fails loudly. Islands are dropped at commit.
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    for (auto [bbID, _] : bbc.bbs()) {
      auto a = ctx.storage.nodes.get_args(bbID);
      for (size_t i = 0; i < a.size(); i++) {
        auto tag = IRI_NODE(ctx, a[i]).tag;
        if (tag == IRI_TAG::InvokeFinalizer)
          throw std::runtime_error(
              "_20_IFB: InvokeFinalizer survived inlining");
        if (tag == IRI_TAG::Ret) {
          if (i + 1 != a.size())
            throw std::runtime_error("_20_IFB: Ret inside a BB");
          ctx.storage.nodes.update_arg_inplace(
              bbID, i,
              ThrowSEXP::create(
                  ctx, StringSEXP::create(
                           ctx, ctx.storage.strings.intern("Unreachable"))));
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
