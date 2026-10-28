#include "Entrypoint.h"
#include <system_error>
#include <filesystem>
#include <cstdlib>
#include "Analysis/ConstantProp.h"
#include "Analysis/DCE.h"
#include "Analysis/EffectProp.h"
#include "Analysis/MTDZS.h"
#include "Analysis/ReduceComputedFieldOps.h"
#include "CorePasses.h"
#include "Helpers.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include "Support/ClosureTree.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include "Support/PTA.hpp"
#include "Storage/GCStats.h"
#include <exception>
#include <iostream>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <cstdlib>

namespace IRI_ENTRY {
using namespace IRI_STORAGE;
using namespace IRI_PARSE;
using namespace IRI_STRUCTURAL;

// 0 = no GC, 1 = GC only after core passes, 2 = GC only after iris commit, 3 = both (default)
inline int gcMode() {
  static const int m = [] {
    const char *e = std::getenv("IRI_GC");
    return e ? std::atoi(e) : 3;
  }();
  return m;
}

// record process VmRSS at a particular stage
inline void emitPhase(IRIContext & ctx, const std::string &name) {
  if (!statsOn()) return;
  auto m = ctx.storage.nodes.memoryUsage();
  std::cerr << "IRI_PHASE," << name << "," << m.usedBytes << "," << m.capBytes
            << "," << procStatusKb("VmRSS") << "," << procStatusKb("VmHWM") << "\n";
}

using BuildContextMap =
    std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

namespace {

void dumpIfEnabled(IRIContext &ctx, IRID sexp, const std::string &name) {
  if (ctx.flags.dumpForgePasses) {
    std::ofstream outFile(name + ".iridump");
    IRI_NODE(ctx, sexp).dumpFlat(outFile, &ctx);
  }
}

void runCorePasses(IRIContext &ctx, IRID & sexpRef, BuildContextMap &buildContext) {
  dumpIfEnabled(ctx, sexpRef, "_0_BEFORE_NORMALIZAION");

  IRI_CORE_PASSES::CTARGETS continueTargets;

  struct CorePassEntry {
    const char *name;
    std::function<void()> run;
  };

  std::vector<CorePassEntry> passes = {
      {"_1_NBBF", [&] { IRI_CORE_PASSES::_1_NBBF(ctx, sexpRef, buildContext); }},
      {"_2_HFD", [&] { IRI_CORE_PASSES::_2_HFD(ctx, sexpRef, buildContext); }},
      {"_4_1_1_GICG",
       [&] { IRI_CORE_PASSES::_4_1_1_GICG(ctx, sexpRef, buildContext); }},
      {"_4_1_2_RUR",
       [&] { IRI_CORE_PASSES::_4_1_2_RUR(ctx, sexpRef, buildContext); }},
      {"_4_2_PMB", [&] { IRI_CORE_PASSES::_4_2_PMB(ctx, sexpRef, buildContext); }},
      {"_4_3_PIB", [&] { IRI_CORE_PASSES::_4_3_PIB(ctx, sexpRef, buildContext); }},
      {"_4_4_RFD", [&] { IRI_CORE_PASSES::_4_4_RFD(ctx, sexpRef, buildContext); }},
      {"_4_5_PEB", [&] { IRI_CORE_PASSES::_4_5_PEB(ctx, sexpRef, buildContext); }},
      {"_6_PHCSC", [&] { IRI_CORE_PASSES::_6_PHCSC(ctx, sexpRef, buildContext); }},
      {"_7_RRPEBS",
       [&] { IRI_CORE_PASSES::_7_RRPEBS(ctx, sexpRef, buildContext); }},
      {"_8_RREBS", [&] { IRI_CORE_PASSES::_8_RREBS(ctx, sexpRef, buildContext); }},
      {"_9_CER", [&] { IRI_CORE_PASSES::_9_RLT(ctx, sexpRef, buildContext); }},
      {"_10_RBACT",
       [&] {
         continueTargets = IRI_CORE_PASSES::_10_RBACT(ctx, sexpRef, buildContext);
       }},
      {"_11_12_DAPRT",
       [&] { IRI_CORE_PASSES::_11_12_DAPRT(ctx, sexpRef, buildContext); }},
      {"_16_MDE", [&] { IRI_CORE_PASSES::_16_MDE(ctx, sexpRef, buildContext); }},
      {"_17_RTDZ", [&] { IRI_CORE_PASSES::_17_RTDZ(ctx, sexpRef, buildContext); }},
      {"_18_DELOP",
       [&] { IRI_CORE_PASSES::_18_DELOP(ctx, sexpRef, buildContext); }},
      {"_19_CBBAMTLA",
       [&] { IRI_CORE_PASSES::_19_CBBAMTLA(ctx, sexpRef, buildContext); }},
      {"_21_TER", [&] { IRI_CORE_PASSES::_21_TER(ctx, sexpRef, buildContext); }},
      {"_22_ESTKTHM",
       [&] {
         IRI_CORE_PASSES::_22_ESTKTHM(ctx, sexpRef, buildContext, continueTargets);
       }},
      {"_AFTER_CORE_PASSES_",
       [&] { IRI_CORE_PASSES::_FNOPS(ctx, sexpRef, buildContext); }},
  };
  
  for (auto &pass : passes) {
    ctx.debugger.tick(pass.name);
    pass.run();
    ctx.debugger.tock(pass.name);
    dumpIfEnabled(ctx, sexpRef, pass.name);
    emitPhase(ctx,pass.name);
  }

  if (gcMode() & 1) {
    auto compaction_result = ctx.storage.nodes.garbageCollect(ctx, sexpRef, "AFTER_CORE_PASSES");
  }

  emitPhase(ctx, "post_core_gc_point");
}

void buildClosureTree(IRIContext &ctx, IRID& sexpRef) {
  IRID treeRoot = IRI_HELPERS::getTopLevelContainer(ctx, sexpRef);
  ctx.closureTree = std::make_unique<ClosureTree>(ctx, treeRoot);
  FileSupport fs(sexpRef, ctx);

  for (auto [bbcID, _] : fs.containers()) {
    if (bbcID == treeRoot)
      continue;
    ctx.closureTree->addClosure(bbcID);
  }

  ctx.iris->populateCClosuresInTree();
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
  ctx.closureTree->preorderTraversal(
      [&](IRICFG *cfgCTX) { pm.run(*cfgCTX, am); });
  ctx.debugger.tock("_23_OPT_MTDZS_DCE");

  if (ctx.flags.pta) {
    ctx.debugger.tick("_25_PTA");
    try {
      PTASolver::solve(ctx);
    } catch (const std::exception &e) {
      std::cerr << "[PTA] " << e.what() << std::endl;
    }
    ctx.debugger.tock("_25_PTA");
  }

  if (ctx.flags.effectProp) {
    ctx.debugger.tick("_24_OPT_EFFECT_PROP");
    ctx.closureTree->preorderTraversal([&](IRICFG *cfgCTX) {
      EffectPropPass ep;
      bool changed = true;
      int iterations = 0;
      const int maxIterations = 50;
      while (changed && iterations < maxIterations) {
        changed = ep.run(*cfgCTX, am);
        iterations++;
      }
    });
    ctx.debugger.tock("_24_OPT_EFFECT_PROP");
  }
}

// One DOT file per closure in out.cfg, rendered to PNG when graphviz is around.
void dumpCFGs(IRIContext &ctx) {
  const std::string dir = "out.cfg";
  std::error_code ec;
  std::filesystem::create_directories(dir, ec);
  if (ec) {
    std::cerr << "[dumpCFG] cannot create " << dir << ": " << ec.message()
              << std::endl;
    return;
  }

  // Probed once: without graphviz the DOT files are still written.
  static const bool haveDot =
      std::system("command -v dot > /dev/null 2>&1") == 0;

  size_t n = 0;
  ctx.closureTree->preorderTraversal([&](IRICFG *cfg) {
    std::string stem = dir + "/c" + std::to_string(n++);
    cfg->dumpDOT(stem + ".dot");
    if (haveDot)
      std::system(("dot -Tpng " + stem + ".dot -o " + stem + ".png"
                   " 2> /dev/null")
                      .c_str());
  });
  std::cout << "[dumpCFG] wrote " << n << " CFG" << (n == 1 ? "" : "s")
            << " to " << dir << (haveDot ? " (.dot + .png)" : " (.dot only)")
            << std::endl;
}

void commitAndDump(IRIContext &ctx, IRID& sexpRef) {
  ctx.closureTree->commit();
  ctx.iris->commit();

  dumpIfEnabled(ctx, sexpRef, "AFTER_OPT_PASSES");
  if (ctx.flags.dumpCFG)
    dumpCFGs(ctx);
  if (ctx.flags.dumpClosureTree)
    ctx.closureTree->dumpFlat(std::cout);
  if (ctx.flags.dumpIrisInfo)
    ctx.iris->dumpFlat(std::cout);
  if (ctx.flags.debugStorage)
    ctx.storage.dumpDiagnostics();
}

} // namespace

void normalizeIRIDIUM(IRIContext &ctx, IRID& sexpRef,
                      BuildContextMap &iridiumBuildContext) {
  runCorePasses(ctx, sexpRef, iridiumBuildContext);
  buildClosureTree(ctx, sexpRef);
  runOptimizationPasses(ctx);
  commitAndDump(ctx, sexpRef);
}

IRID sharedEntrypoint(IRIContext &ctx, IRID& file,
                      BuildContextMap &iridiumBuildContext) {
                      
  if (statsOn()) {
     resetPeakRss();   // VmHWM measures peak for normalization + GC

     std::cerr << "IRI_SIZES," << sizeof(IRI_STORAGE::IridiumSEXP) << "," << sizeof(IRID)
               << "," << sizeof(FlagValue) << "\n";
     std::cerr << "IRI_START," << procStatusKb("VmRSS") << "," << procStatusKb("VmHWM") << "\n";
    emitPhase(ctx, "start");
  }

  normalizeIRIDIUM(ctx, file, iridiumBuildContext);

  emitPhase(ctx, "pre_final_gc_point");
  if (gcMode() & 2) {
    auto compaction_result = ctx.storage.nodes.garbageCollect(ctx, file, "AFTER_IRIS_COMMIT");
  }
  emitPhase(ctx, "post_final_gc_point");

  if (statsOn())
    std::cerr << "IRI_FINAL," << procStatusKb("VmRSS") << "," << procStatusKb("VmHWM") << "\n";

  return file;
}

} // namespace IRI_ENTRY
