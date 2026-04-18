#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline IRID reduceJSFuncDecl(IridiumPool &pool, IRID funcDeclID) {
  JSFuncDeclSEXP funcDecl(funcDeclID, pool);
  return JSExplicitBindingDeclarationSEXP::create(
      pool, funcDecl.getArg_LValTarget(), funcDecl.getArg_RVal(), false, false,
      true, false, true, false);
}

void _4_4_RFD(IridiumPool &pool, IRID fileID, BUILD_CTX &iridiumBuildContext) {
  FileSEXP fileSEXP(fileID, pool);

  double topLevelScope = IRI_HELPERS::getTopLevelScope(pool, fileID);
  bool isModule = iridiumBuildContext[topLevelScope]->isModule;

  auto args = pool.get_args(fileID);

  for (auto &bbcID : args) {
    if (pool[bbcID].tag != IRI_GEN::BBContainer)
      continue;

    BBContainerSEXP container(bbcID, pool);
    auto bbs = pool.get_args(container.getArg_BB());
    auto containerScope = container.getScopeIDX();

    for (auto &bbID : bbs) {
      BBSEXP bb(bbID, pool);
      auto stmts = pool.get_args(bbID);
      for (size_t i = 0; i < stmts.size(); i++) {
        IRID stmtID = stmts[i];
        IRI_TAG currTag = pool[stmtID].tag;

        if (currTag == IRI_GEN::JSFuncDecl) {
          if (!isModule && containerScope == topLevelScope) {
            // NADA
          } else {
            pool.update_arg_inplace(bbID, i, reduceJSFuncDecl(pool, stmtID));
          }
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
