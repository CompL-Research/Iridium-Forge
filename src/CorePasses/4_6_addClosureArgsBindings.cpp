#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
#include <cstdlib>
#include <unordered_map>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _4_6_CBA(IridiumPool &pool, IRID fileID, BUILD_CTX &iridiumBuildContext) {
  FileSEXP fileSEXP(fileID, pool);

  auto args = pool.get_args(fileID);

  for (auto &bbcID : args) {
    if (pool[bbcID].tag != IRI_GEN::BBContainer)
      continue;

    BBContainerSEXP container(bbcID, pool);

    auto &containerBC = iridiumBuildContext[container.getScopeIDX()];

    BindingsSEXP bindingsSEXP(container.getArg_Bindings(), pool);
    std::vector<IRID> localBindings;

    for (int k = 0; k < containerBC->args.size(); k++) {
      IRI_GEN::IRI_FLAG flag = IRI_GEN::IRI_FLAG::JSARG;
      if (k + 1 == containerBC->args.size() && containerBC->hasRestArgs) {
        flag = IRI_GEN::IRI_FLAG::JSRESTARG;
      }

      auto res = EnvBindingSEXP::create(pool,
          pool.strings.intern(containerBC->args[k]), false, flag == IRI_GEN::IRI_FLAG::JSARG,
          flag == IRI_GEN::IRI_FLAG::JSRESTARG, false, false, false, false,
          containerBC->scopeIDX, k, containerBC->scopeIDX, containerBC->parent,
          -1);

      localBindings.push_back(res);
    }
    pool.add_args_to_end(bindingsSEXP.getArg_LocalBindings(), localBindings);
  }
}
} // namespace IRI_CORE_PASSES
