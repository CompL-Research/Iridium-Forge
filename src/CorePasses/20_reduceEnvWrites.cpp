

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/BindingsPool.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <memory>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _20_REW(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
                 BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);

      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        IRI_TAG stmtTAG = pool[stmtID].tag;

        if (stmtTAG == IRI_GEN::EnvWrite) {
          EnvWriteSEXP ewSEXP(stmtID, pool);
          IRID lval = ewSEXP.getArg_LValTarget();
          IRID rval = ewSEXP.getArg_RVal();

          bool SLOPPY   = ewSEXP.hasSLOPPY();
          bool TAINTED  = false;
          bool SAFE     = ewSEXP.getSAFE();
          bool THISINIT = ewSEXP.getTHISINIT();

          if (pool[lval].tag == IRI_GEN::GlobalBinding) {
            pool.update_arg_inplace(bbID, stmtOffset, GWriteSEXP::create(pool, lval, rval, SLOPPY, TAINTED, SAFE));
          } else if (pool[lval].tag == IRI_GEN::EnvBinding) {
            pool.update_arg_inplace(bbID, stmtOffset, LWriteSEXP::create(pool, lval, rval, SLOPPY, SAFE, THISINIT));
          } else if (pool[lval].tag == IRI_GEN::RemoteEnvBinding) {
            pool.update_arg_inplace(bbID, stmtOffset, RWriteSEXP::create(pool, lval, rval, SLOPPY, TAINTED, SAFE, THISINIT));
          } else {
            assert(false && "Unexpected store target for EnvWrite");
          }
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
