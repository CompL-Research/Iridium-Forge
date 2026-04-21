

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <algorithm>
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

void _19_CBB(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
             BUILD_CTX &iridiumBuildContext) {
  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);
      std::vector<IRID> newBB = pool.get_args(bbID);
      pool.update_num_args(bbID, CBB(pool, newBB));
    }
  }
}
} // namespace IRI_CORE_PASSES
