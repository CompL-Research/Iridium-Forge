
#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;

void _3_FNOPS(
    IridiumPool &pool, IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) {

  const auto bbs = pool.get_args(fileSEXP);

  for (auto &currBBID : bbs)
    pool.remove_args_matching_tag(currBBID, IRI_TAG::Null);
}
} // namespace IRI_CORE_PASSES
