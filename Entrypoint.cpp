#include "Entrypoint.h"
#include "Analysis/ConstantProp.h"
#include "Analysis/DCE.h"
#include "Analysis/EffectProp.h"
#include "Analysis/MTDZS.h"
#include "Analysis/ReduceComputedFieldOps.h"
#include "CorePasses.h"
#include "Helpers.h"
#include "Storage/Config.h"
#include "Storage/GCStats.h"
#include "Storage/IRIContext.h"
#include "Support/ClosureTree.hpp"
#include "Support/IRIS.hpp"
#include "Support/PP.hpp"
#include "Support/TryStructure.hpp"
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <system_error>
#include <unordered_map>
#include <vector>

namespace IRI_ENTRY {
using namespace IRI_STORAGE;
using namespace IRI_PARSE;
using namespace IRI_STRUCTURAL;

// 0 = no GC, 1 = GC only after core passes, 2 = GC only after iris commit, 3 =
// both (default)
inline int gcMode() {
  static const int m = [] {
    const char *e = std::getenv("IRI_GC");
    return e ? std::atoi(e) : 3;
  }();
  return m;
}

// record process VmRSS at a particular stage
inline void emitPhase(IRIContext &ctx, const std::string &name) {
  if (!statsOn())
    return;
  auto m = ctx.storage.nodes.memoryUsage();
  std::cerr << "IRI_PHASE," << name << "," << m.usedBytes << "," << m.capBytes
            << "," << procStatusKb("VmRSS") << "," << procStatusKb("VmHWM")
            << "\n";
}

using BuildContextMap =
    std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

namespace {

void dumpIfEnabled(IRIContext &ctx, IRID sexp, const std::string &name) {
  if (ctx.flags.dumpForgePassesSEXP) {
    std::ofstream outFile("out.iri/" + name + ".iri");
    IRI_NODE(ctx, sexp).dumpFlat(outFile, &ctx);
  }

  if (ctx.flags.dumpForgePasses) {
    // Readable form of the snapshots worth reading by hand.
    if (name == "AFTER_CORE_PASSES" || name == "AFTER_OPT_PASSES") {
      std::ofstream ppFile("out.iri/" + name + ".pp.iri");
      prettyPrint(ppFile, ctx, sexp);
    }
  }
}

void runCorePasses(IRIContext &ctx, IRID &sexpRef,
                   BuildContextMap &buildContext) {
  if (ctx.flags.dumpForgePasses || ctx.flags.dumpForgePassesSEXP) {
    namespace fs = std::filesystem;
    std::filesystem::path root_ = fs::path("out.iri");
    fs::create_directories(root_);
  }

  dumpIfEnabled(ctx, sexpRef, "_0_BEFORE_NORMALIZAION");

  IRI_CORE_PASSES::CTARGETS continueTargets;

  struct CorePassEntry {
    const char *name;
    std::function<void()> run;
  };

  std::vector<CorePassEntry> passes = {
      {"_1_NBBF",
       [&] { IRI_CORE_PASSES::_1_NBBF(ctx, sexpRef, buildContext); }},
      {"_2_HFD", [&] { IRI_CORE_PASSES::_2_HFD(ctx, sexpRef, buildContext); }},
      {"_4_1_1_GICG",
       [&] { IRI_CORE_PASSES::_4_1_1_GICG(ctx, sexpRef, buildContext); }},
      {"_4_1_2_RUR",
       [&] { IRI_CORE_PASSES::_4_1_2_RUR(ctx, sexpRef, buildContext); }},
      {"_4_2_PMB",
       [&] { IRI_CORE_PASSES::_4_2_PMB(ctx, sexpRef, buildContext); }},
      {"_4_3_PIB",
       [&] { IRI_CORE_PASSES::_4_3_PIB(ctx, sexpRef, buildContext); }},
      {"_4_4_RFD",
       [&] { IRI_CORE_PASSES::_4_4_RFD(ctx, sexpRef, buildContext); }},
      {"_4_5_PEB",
       [&] { IRI_CORE_PASSES::_4_5_PEB(ctx, sexpRef, buildContext); }},
      {"_8_RREBS",
       [&] { IRI_CORE_PASSES::_8_RREBS(ctx, sexpRef, buildContext); }},
      {"_10_RBACT",
       [&] {
         continueTargets =
             IRI_CORE_PASSES::_10_RBACT(ctx, sexpRef, buildContext);
       }},
      {"_11_12_DAPRT",
       [&] { IRI_CORE_PASSES::_11_12_DAPRT(ctx, sexpRef, buildContext); }},
      {"_16_MDE",
       [&] { IRI_CORE_PASSES::_16_MDE(ctx, sexpRef, buildContext); }},
      {"_17_RTDZ",
       [&] { IRI_CORE_PASSES::_17_RTDZ(ctx, sexpRef, buildContext); }},
      {"_18_DELOP",
       [&] { IRI_CORE_PASSES::_18_DELOP(ctx, sexpRef, buildContext); }},
      {"_19_CBBAMTLA",
       [&] { IRI_CORE_PASSES::_19_CBBAMTLA(ctx, sexpRef, buildContext); }},
      {"_20_IFB",
       [&] {
         // Lexical try data is only needed to materialize BB.EXCIDX; the CFG
         // owns exception flow from here on, so it ends with this pass.
         TryStructure tryInfo(*ctx.iris, buildContext);
         IRI_CORE_PASSES::_20_IFB(ctx, sexpRef, buildContext, continueTargets,
                                  tryInfo);
       }},
      {"_21_TER",
       [&] { IRI_CORE_PASSES::_21_TER(ctx, sexpRef, buildContext); }},
      {"_22_ESTKTHM",
       [&] {
         IRI_CORE_PASSES::_22_ESTKTHM(ctx, sexpRef, buildContext,
                                      continueTargets);
       }},
      {"_23_APVK",
       [&] { IRI_CORE_PASSES::_23_APVK(ctx, sexpRef, buildContext); }},
      {"AFTER_CORE_PASSES",
       [&] { IRI_CORE_PASSES::_FNOPS(ctx, sexpRef, buildContext); }},
  };

  for (auto &pass : passes) {
    ctx.debugger.tick(pass.name);
    pass.run();
    ctx.debugger.tock(pass.name);
    dumpIfEnabled(ctx, sexpRef, pass.name);
    emitPhase(ctx, pass.name);
  }

  if (gcMode() & 1) {
    auto compaction_result =
        ctx.storage.nodes.garbageCollect(ctx, sexpRef, "AFTER_CORE_PASSES");
  }

  emitPhase(ctx, "post_core_gc_point");
}

void runOptimizationPasses(IRIContext &ctx) {
  AnalysisManager am;
  PassManager pm;

  if (ctx.flags.constantProp) {
    pm.addPass(ConstantPropPass());
    // After constant propagation, so a key that is only constant once folded
    // still reduces. Before PTA, which relies on the static form.
    pm.addPass(ReduceComputedFieldOpsPass());
  }
  if (ctx.flags.tdz)
    pm.addPass(MTDZSPass());
  if (ctx.flags.dce)
    pm.addPass(DCEPass());

  ctx.debugger.tick("_23_OPT_MTDZS_DCE");
  for (IRICFG *cfgCTX : ctx.closureTree->preorder())
    pm.run(*cfgCTX, am);
  ctx.debugger.tock("_23_OPT_MTDZS_DCE");

  if (ctx.flags.pta) {
    throw std::runtime_error("PTA WIP");
    // ctx.debugger.tick("_25_PTA");
    // try {
    //   PTASolver::solve(ctx);
    // } catch (const std::exception &e) {
    //   std::cerr << "[PTA] " << e.what() << std::endl;
    // }
    // ctx.debugger.tock("_25_PTA");
  }

  if (ctx.flags.effectProp) {
    ctx.debugger.tick("_24_OPT_EFFECT_PROP");
    for (IRICFG *cfgCTX : ctx.closureTree->preorder()) {
      EffectPropPass ep;
      bool changed = true;
      int iterations = 0;
      const int maxIterations = 50;
      while (changed && iterations < maxIterations) {
        changed = ep.run(*cfgCTX, am);
        iterations++;
      }
    }
    ctx.debugger.tock("_24_OPT_EFFECT_PROP");
  }
}

void dumpCFGs(IRIContext &) {
  throw std::runtime_error("[dumpCFG] WIP: dumpCFG");
}

void commitAndDump(IRIContext &ctx, IRID &sexpRef) {
  ctx.closureTree->commit();
  ctx.iris->commit();

  dumpIfEnabled(ctx, sexpRef, "AFTER_OPT_PASSES");
  if (ctx.flags.dumpCFG)
    dumpCFGs(ctx);
  if (ctx.flags.dumpClosureTree)
    throw std::runtime_error("[dumpClosure] WIP: disabled for now");
  if (ctx.flags.dumpIrisInfo)
    ctx.iris->dumpFlat(std::cout);
  if (ctx.flags.debugStorage)
    ctx.storage.dumpDiagnostics();
}

} // namespace

void normalizeIRIDIUM(IRIContext &ctx, IRID &sexpRef,
                      BuildContextMap &iridiumBuildContext) {
  runCorePasses(ctx, sexpRef, iridiumBuildContext);
  ctx.closureTree = std::make_unique<ClosureTree>(ctx, sexpRef);
}

IRID sharedEntrypoint(IRIContext &ctx, IRID &file,
                      BuildContextMap &iridiumBuildContext) {

  if (statsOn()) {
    resetPeakRss(); // VmHWM measures peak for normalization + GC

    std::cerr << "IRI_SIZES," << sizeof(IRI_STORAGE::IridiumSEXP) << ","
              << sizeof(IRID) << "," << sizeof(FlagValue) << "\n";
    std::cerr << "IRI_START," << procStatusKb("VmRSS") << ","
              << procStatusKb("VmHWM") << "\n";
    emitPhase(ctx, "start");
  }

  normalizeIRIDIUM(ctx, file, iridiumBuildContext);
  runOptimizationPasses(ctx);
  commitAndDump(ctx, file);

  emitPhase(ctx, "pre_final_gc_point");
  if (gcMode() & 2) {
    auto compaction_result =
        ctx.storage.nodes.garbageCollect(ctx, file, "AFTER_IRIS_COMMIT");
  }
  emitPhase(ctx, "post_final_gc_point");

  if (statsOn())
    std::cerr << "IRI_FINAL," << procStatusKb("VmRSS") << ","
              << procStatusKb("VmHWM") << "\n";

  return file;
}

} // namespace IRI_ENTRY
