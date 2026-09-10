#include "Entrypoint.h"
#include "Analysis/ConstantProp.h"
#include "Analysis/DCE.h"
#include "Analysis/EffectProp.h"
#include "Analysis/MTDZS.h"
#include "CorePasses.h"
#include "Helpers.h"
#include "IRIPerf.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Support/ClosureTree.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include "Support/PTA.hpp"
#include <exception>
#if DUMP_CORE_PASSES == 1
#include <fstream>
#include <sstream>
#endif
#include <memory>
#include <unordered_map>

#include <cstdlib>

namespace IRI_ENTRY {
using namespace IRI_STORAGE;
using namespace IRI_PARSE;
using namespace IRI_STRUCTURAL;

inline void
normalizeIRIDIUM(IridiumPool &pool, IRID sexp,
                 std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>
                     &iridiumBuildContext,
                 IRIPerf &iriPerf) {

  // Helper to dump state
  auto dump = [&](const std::string &name) {
#if DUMP_CORE_PASSES == 1
    std::ofstream outFile(name + ".iridump");
    pool[sexp].dumpFlat(outFile, &pool);
#endif
  };

  // Helper to run a pass
  auto runPass = [&](const std::string &name, auto passFunc) {
    iriPerf.tick(name);
    passFunc(pool, sexp, iridiumBuildContext);
    iriPerf.tock(name);
    dump(name);
  };

  dump("_0_BEFORE_NORMALIZAION");

  // Execute passes
  runPass("_1_NBBF", IRI_CORE_PASSES::_1_NBBF);
  runPass("_2_HFD", IRI_CORE_PASSES::_2_HFD);
  runPass("_4_1_1_GICG", IRI_CORE_PASSES::_4_1_1_GICG);
  runPass("_4_1_2_RUR", IRI_CORE_PASSES::_4_1_2_RUR);
  runPass("_4_2_PMB", IRI_CORE_PASSES::_4_2_PMB);
  runPass("_4_3_PIB", IRI_CORE_PASSES::_4_3_PIB);
  runPass("_4_4_RFD", IRI_CORE_PASSES::_4_4_RFD);
  runPass("_4_5_PEB", IRI_CORE_PASSES::_4_5_PEB);
  runPass("_6_PHCSC", IRI_CORE_PASSES::_6_PHCSC);
  runPass("_7_RRPEBS", IRI_CORE_PASSES::_7_RRPEBS);
  runPass("_8_RREBS", IRI_CORE_PASSES::_8_RREBS);
  runPass("_9_CER", IRI_CORE_PASSES::_9_RLT);
  runPass("_10_RBACT", IRI_CORE_PASSES::_10_RBACT);
  runPass("_11_12_DAPRT", IRI_CORE_PASSES::_11_12_DAPRT);
  runPass("_16_MDE", IRI_CORE_PASSES::_16_MDE);
  runPass("_17_RTDZ", IRI_CORE_PASSES::_17_RTDZ);
  runPass("_18_DELOP", IRI_CORE_PASSES::_18_DELOP);
  runPass("_19_CBBAMTLA", IRI_CORE_PASSES::_19_CBBAMTLA);
  runPass("_21_TER", IRI_CORE_PASSES::_21_TER);
  runPass("_22_ESTKTHM", IRI_CORE_PASSES::_22_ESTKTHM);
  runPass("_AFTER_CORE_PASSES_", IRI_CORE_PASSES::_FNOPS);

  // Initialize closure tree and create CFGs
  IRID treeRoot = IRI_HELPERS::getTopLevelContainer(pool, sexp);
  pool.closureTree = std::make_unique<ClosureTree>(pool, treeRoot);
  FileSupport fs(sexp, pool);

  for (auto [bbcID, _] : fs.containers()) {
    if (bbcID == treeRoot)
      continue;
    pool.closureTree->addClosure(bbcID);
  }

  // Must happen after the tree is created and all closures have already been
  // added
  pool.iris->populateCClosuresInTree();

  AnalysisManager am;
  PassManager pm;

  if (std::getenv("NOCP")) {
    // Skip Pass
  } else {
    pm.addPass(ConstantPropPass());
  }

  if (std::getenv("NOTDZ")) {
    // Skip Pass
  } else {
    pm.addPass(MTDZSPass());
  }

  if (std::getenv("NODCE")) {
    // Skip Pass
  } else {
    pm.addPass(DCEPass());
  }

  iriPerf.tick("_23_OPT_MTDZS_DCE");
  pool.closureTree->preorderTraversal(
      [&](IRICFG *cfgCTX) { pm.run(*cfgCTX, am); });
  iriPerf.tock("_23_OPT_MTDZS_DCE");

  if (std::getenv("NOEP")) {
    // Skip Pass
  } else {
    iriPerf.tick("_24_OPT_EFFECT_PROP");
    pool.closureTree->preorderTraversal([&](IRICFG *cfgCTX) {
      EffectPropPass ep;
      bool changed = true;
      int iterations = 0;
      const int maxIterations = 50;
      BBContainerSupport bbc(cfgCTX->id, cfgCTX->pool);
      // std::cout << "[EffectProp] Optimization starting for closure " <<
      // bbc.getStartBBIDX() << "...\n";
      while (changed && iterations < maxIterations) {
        // auto start = std::chrono::high_resolution_clock::now();
        changed = ep.run(*cfgCTX, am);
        // auto end = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double, std::milli> elapsed = end - start;
        // std::cout << "  - Iteration " << (iterations + 1) << ": "
        //           << (changed ? "changed" : "no change")
        //           << " (" << elapsed.count() << " ms)\n";
        iterations++;
      }
    });
    iriPerf.tock("_24_OPT_EFFECT_PROP");
  }

  try {
    // Perform PTA
    PTASolver ptaSolver;
    ptaSolver.solve(pool);
  } catch (std::exception e) {
    std::cout << "PTA failed" << std::endl;
  }
  // Commit closure level bindings
  pool.closureTree->commit();

#if DUMP_ANALYSIS_RESULTS == 1
  {
    std::ofstream outFile("ANALYSIS_DUMP.iridump");
    pool.closureTree->preorderTraversal([&](IRICFG *cfgCTX) {
      BBContainerSupport bbc(cfgCTX->id, cfgCTX->pool);
      outFile << "Closure " << bbc.getStartBBIDX() << ":\n";

      // Query both analyses to populate them in the cache
      am.getResult<TDZAnalysis>(*cfgCTX);
      am.getResult<LivenessAnalysis>(*cfgCTX);
      am.getResult<ConstantsAtStmt>(*cfgCTX);
      am.getResult<EffectAtStmtAnalysis>(*cfgCTX);

      // Interleave and print all cached dataflow analyses dynamically
      am.dumpDataflowStates(*cfgCTX, outFile);
      outFile << "\n";
    });
  }
#endif

  pool.iris->commit();

  dump("AFTER_OPT_PASSES");
  pool.closureTree->dumpFlat(std::cout);
  pool.iris->dumpFlat(std::cout);
}

IRID sharedEntrypoint(
    IridiumPool &pool, IRID file,
    std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>
        &iridiumBuildContext,
    IRIPerf &iriPerf) {
  normalizeIRIDIUM(pool, file, iridiumBuildContext, iriPerf);

  return file;
}

} // namespace IRI_ENTRY
