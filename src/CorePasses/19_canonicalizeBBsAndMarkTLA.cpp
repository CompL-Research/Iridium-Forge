

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <stdexcept>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline static bool isCFlowStmt(IRI_TAG tag) {
  if (ReturnAsync == tag)
    return true;
  if (Goto == tag)
    return true;
  if (IfElseJump == tag)
    return true;
  if (Ret == tag)
    return true;
  if (Return == tag)
    return true;
  if (Throw == tag)
    return true;
  return false;
}

inline static size_t CBB(IRI_STORAGE::IridiumPool &pool, std::vector<IRID> &v) {
  if (v.size() == 0)
    throw std::runtime_error("Found a BB with zero statements, unexpected");

  for (size_t i = 0; i < v.size(); i++) {
    if (isCFlowStmt(pool[v[i]].tag))
      return i + 1;
  }
  throw std::runtime_error("Found a BB with no control flow statement");
}

void _19_CBBAMTLA(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
             BUILD_CTX &iridiumBuildContext) {
  assert(pool.topLevelBBContainer.has_value() &&
         "Expected that the pool is aware about the top level container...");
  IRID topLevelContainer = pool.topLevelBBContainer.value();
  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);
      std::vector<IRID> newBB = pool.get_args(bbID);
      //
      // Mark TLA before it is possibly deleted
      //
      if (bbcID == topLevelContainer) {
        for (auto [stmtID, _] : bb.stmts()) {
          auto currTag = pool[stmtID].tag;
          if (IRI_HELPERS::hasNodeWithPredicate(stmtID, &pool, [&](IRID id) { return pool[id].tag == IRI_GEN::Await; })) {
            file.setTLA();
            break;
          }
        }
      }

        pool.update_num_args(bbID, CBB(pool, newBB));
    }
  }
}
} // namespace IRI_CORE_PASSES
