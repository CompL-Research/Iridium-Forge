#include "IRIFlags.hpp"
#include "Entrypoint.h"
#include "CorePasses.h"
#include "Storage/IridiumPool.h"
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
  runPass("_3_FNOPS", IRI_CORE_PASSES::_3_FNOPS);
  runPass("_4_1_GICG", IRI_CORE_PASSES::_4_1_GICG);
  runPass("_4_2_PMB", IRI_CORE_PASSES::_4_2_PMB);
  runPass("_4_3_PIB", IRI_CORE_PASSES::_4_3_PIB);
  runPass("_4_4_RFD", IRI_CORE_PASSES::_4_4_RFD);
  runPass("_4_5_1_PEB", IRI_CORE_PASSES::_4_5_PEB);
  runPass("_4_5_2_FNOPS", IRI_CORE_PASSES::_3_FNOPS);
  runPass("_4_6_CBA", IRI_CORE_PASSES::_4_6_CBA);
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
