#include "Entrypoint.h"
#include "CorePasses.h"
#include "Generated/IridiumTypes.h"
#include "IRIPerf.h"
#include "Storage/Config.h"
#include <fstream>
#include <memory>
#include <unordered_map>

namespace IRI_ENTRY {

inline void normalizeIRIDIUM(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID sexp,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext,
    IRIPerf &iriPerf) {
  {
    std::ofstream outFile("_0_BEFORE_NORMALIZAION.irdump");
    pool[sexp].dumpFlat(outFile, &pool);
  }

  iriPerf.tick("_1_NBBF");
  IRI_CORE_PASSES::_1_NBBF(pool, sexp, iridiumBuildContext);
  iriPerf.tock("_1_NBBF");

  {
    std::ofstream outFile("_1_NBBF.irdump");
    pool[sexp].dumpFlat(outFile, &pool);
  }
  iriPerf.tick("_2_HFD");
  IRI_CORE_PASSES::_2_HFD(pool, sexp, iridiumBuildContext);
  iriPerf.tock("_2_HFD");

  {
    std::ofstream outFile("_2_HFD.irdump");
    pool[sexp].dumpFlat(outFile, &pool);
  }
  iriPerf.tick("_3_FNOPS");
  IRI_CORE_PASSES::_3_FNOPS(pool, sexp, iridiumBuildContext);
  iriPerf.tock("_3_FNOPS");

  {
    std::ofstream outFile("_3_FNOPS.irdump");
    pool[sexp].dumpFlat(outFile, &pool);
  }
  iriPerf.tick("_4_GICG");
  IRI_CORE_PASSES::_4_GICG(pool, sexp, iridiumBuildContext);
  iriPerf.tock("_4_GICG");

  {
    std::ofstream outFile("_4_GICG.irdump");
    pool[sexp].dumpFlat(outFile, &pool);
  }
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
