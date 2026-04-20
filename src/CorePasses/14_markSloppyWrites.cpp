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

void _14_MSW(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
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
          if (!bbc.hasSTRICT()) {
            envWrite.setSLOPPY();
          }

          IRID right = envWrite.getArg_RVal();
          // Eventually this should not be needed
          if (pool[right].tag == EnvWrite){
            EnvWriteSEXP envWrite(right, pool);
            if (!bbc.hasSTRICT()) {
              envWrite.setSLOPPY();
            }
          }
        }

        if (stmtTag == IRI_GEN::JSExplicitBindingDeclaration ||
            stmtTag == IRI_GEN::JSExplicitBindingDeclarationN) {
          throw std::runtime_error(
              "found JSExplicitBindingDeclaration || "
              "JSExplicitBindingDeclarationN, unexpected at this stage!");
        }

        if (stmtTag == IRI_GEN::JSImplicitBindingDeclaration) {
          JSImplicitBindingDeclarationSEXP envWrite(stmtID, pool);
          auto storeID = envWrite.getArg_Store();
          if (pool[storeID].tag == IRI_GEN::GlobalBinding) {
            throw std::runtime_error("Unexpected, JSImplicitBindingDeclaration is writing to a global? how does this happen, investigate!!");
          }
          // TODO: Deprecate this flag, if the above assertion is actually true...
          if (!bbc.hasSTRICT()) {
            envWrite.setSLOPPY();
          }
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
