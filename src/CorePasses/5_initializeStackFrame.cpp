#include "CorePasses.h"
#include "Parser/IridiumBuildContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _5_INITSFRAME(IridiumPool &pool, IRID fileSEXP,
                   BUILD_CTX &iridiumBuildContext) {

  FileSupport fileSupport(fileSEXP, pool);
  for (auto [bbContID, _] : fileSupport.containers()) {
    BBContainerSupport container(bbContID, pool);
    BindingsSupport bindings(container.getArg_Bindings(), pool);

    bindings.balance(iridiumBuildContext);
  }
}
} // namespace IRI_CORE_PASSES
