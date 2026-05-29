#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _FNOPS(IridiumPool &pool, IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {
  FileSupport fileSupport(fileSEXP, pool);
  for (auto [bbContID, _] : fileSupport.containers()) {
    BBContainerSupport container(bbContID, pool);
    double scopeIDX = container.getScopeIDX();
    std::vector<IRID> res;

    for (auto [bbID, _] : container.bbs()) {
      pool.remove_args_matching_tag(bbID, IRI_TAG::NOP);
    }
  }
}
} // namespace IRI_CORE_PASSES
