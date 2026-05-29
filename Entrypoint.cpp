#include "Entrypoint.h"
#include "CorePasses.h"
#include "Storage/IridiumPool.h"
#include "Support/IRIS.hpp"
#include "IRIPerf.h"
#include "Storage/Config.h"
#if DUMP_CORE_PASSES == 1
#include <fstream>
#endif
#include <memory>
#include <unordered_map>

namespace IRI_ENTRY {

inline void normalizeIRIDIUM(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID sexp,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext,
    IRIPerf &iriPerf) {
  // Helper to dump state
  auto dump = [&](const std::string &name) {
    #if DUMP_CORE_PASSES == 1
    std::ofstream outFile(name + ".irdump");
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
  // runPass("_3_FNOPS", IRI_CORE_PASSES::_3_FNOPS);
  runPass("_4_1_1_GICG", IRI_CORE_PASSES::_4_1_1_GICG);
  runPass("_4_1_2_RUR", IRI_CORE_PASSES::_4_1_2_RUR);
  runPass("_4_2_PMB", IRI_CORE_PASSES::_4_2_PMB);
  runPass("_4_3_PIB", IRI_CORE_PASSES::_4_3_PIB);
  runPass("_4_4_RFD", IRI_CORE_PASSES::_4_4_RFD);
  runPass("_4_5_PEB", IRI_CORE_PASSES::_4_5_PEB);
  // runPass("_4_6_CBA", IRI_CORE_PASSES::_4_6_CBA);
  // runPass("_5_INITSFRAME", IRI_CORE_PASSES::_5_INITSFRAME);
  runPass("_6_PHCSC", IRI_CORE_PASSES::_6_PHCSC);
  runPass("_7_RRPEBS", IRI_CORE_PASSES::_7_RRPEBS);
  runPass("_8_RREBS", IRI_CORE_PASSES::_8_RREBS);
  runPass("_9_CER", IRI_CORE_PASSES::_9_RLT);
  runPass("_10_RBACT", IRI_CORE_PASSES::_10_RBACT);
  runPass("_11_12_DAPRT", IRI_CORE_PASSES::_11_12_DAPRT);
  // runPass("_13_MNSI", IRI_CORE_PASSES::_13_MNSI);
  // runPass("_14_MSW", IRI_CORE_PASSES::_14_MSW);
  // runPass("_15_LWTA", IRI_CORE_PASSES::_15_LWTA);
  runPass("_16_MDE", IRI_CORE_PASSES::_16_MDE);
  runPass("_17_RTDZ", IRI_CORE_PASSES::_17_RTDZ);
  runPass("_18_DELOP", IRI_CORE_PASSES::_18_DELOP);
  runPass("_19_CBBAMTLA", IRI_CORE_PASSES::_19_CBBAMTLA);
  // runPass("_20_REW", IRI_CORE_PASSES::_20_REW);
  runPass("_21_TER", IRI_CORE_PASSES::_21_TER);
  runPass("_22_ESTKTHM", IRI_CORE_PASSES::_22_ESTKTHM);
  // runPass("_23_RIB", IRI_CORE_PASSES::_23_RIB);
  runPass("_CLEANUP_", IRI_CORE_PASSES::_FNOPS);
  pool.iris->commit();
  pool.iris->dumpFlat(std::cout);
  dump("AFTER_CORE_PASSES");
}

IRI_STORAGE::IRID sharedEntrypoint(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID file,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext,
    IRIPerf &iriPerf) {
  normalizeIRIDIUM(pool, file, iridiumBuildContext, iriPerf);
  return file;
}

} // namespace IRI_ENTRY
