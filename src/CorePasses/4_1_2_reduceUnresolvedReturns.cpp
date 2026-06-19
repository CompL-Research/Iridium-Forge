#include "Config.h"
#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include <cstddef>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _4_1_2_RUR(IridiumPool &pool, IRID fileID,
                BUILD_CTX &iridiumBuildContext) {
  StringID undefined_str = pool.strings.intern("undefined");
  StringID this_str = pool.strings.intern("this");
  FileSupport fileSupport(fileID, pool);
  for (auto [bbContID, _] : fileSupport.containers()) {
    BBContainerSupport container(bbContID, pool);

    bool returnThis = container.getContainerFlagID() == CF_DERIVED_CTR;

    for (auto [bbID, _] : container.bbs()) {
      BBSupport bb(bbID, pool);

      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        IRI_TAG currTag = pool[stmtID].tag;

        if (currTag == IRI_GEN::UnresolvedReturn) {
          IRID patched;
          if (returnThis) {
            patched = ReturnSEXP::create(
                pool, IRI_HELPERS::createUnsafeEnvReadSEXP(pool, this_str));
          } else {
            patched = ReturnSEXP::create(
                pool, IRI_HELPERS::createUnsafeEnvReadSEXP(pool, undefined_str));
          }
          pool.update_arg_inplace(bbID, stmtOffset, patched);
        } else if (currTag == IRI_GEN::Return) {
          //
          // Decorate Return statements of derived class constructors
          //
          if (returnThis) {
            ReturnSEXP rSEXP(stmtID, pool);
            pool.update_arg_inplace(bbID, stmtOffset,
              ReturnSEXP::create(
                  pool,
                  DCTRRetSEXP::create(pool, rSEXP.getArg_Obj(), IRI_HELPERS::createUnsafeEnvReadSEXP(pool, this_str))
              )
            );
          }
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
