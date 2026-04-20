
#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <stdexcept>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _15_LWTA(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);

      for (auto [stmtID, _] : bb.stmts()) {
        auto stmtTag = pool[stmtID].tag;
        if (stmtTag == IRI_GEN::EnvWrite) {
          EnvWriteSEXP envWrite(stmtID, pool);
          IRID leftID = envWrite.getArg_LValTarget();
          if (pool[leftID].tag == IRI_GEN::EnvBinding) {
            EnvBindingSEXP leftEnvBinding(leftID, pool);
            if (leftEnvBinding.hasASW())
              envWrite.setSAFE(true);
          }

          IRID right = envWrite.getArg_RVal();
          // Eventually this should not be needed
          if (pool[right].tag == EnvWrite) {
            EnvWriteSEXP envWrite(right, pool);
            IRID leftID = envWrite.getArg_LValTarget();
            if (pool[leftID].tag == IRI_GEN::EnvBinding) {
              EnvBindingSEXP leftEnvBinding(leftID, pool);
              if (leftEnvBinding.hasASW())
                envWrite.setSAFE(true);
            }
          }
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
