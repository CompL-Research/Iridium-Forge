#include "CorePasses.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _13_MNSI(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {
  FileSupport file(fileSEXP, pool);
  for (auto [sImportID, _] : file.staticImports()) {
    StaticImportSEXP sImport(sImportID, pool);
    if (sImport.hasNSIMPORT()) {
      RemoteEnvBindingSEXP uBinding(sImport.getArg_StorageLocation(), pool);
      uBinding.setNSIMPORT();
    }
  }
}
} // namespace IRI_CORE_PASSES
