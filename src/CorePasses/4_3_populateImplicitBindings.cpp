#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include <stdexcept>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _4_3_PIB(IridiumPool &pool, IRID fileID, BUILD_CTX &iridiumBuildContext) {
  FileSEXP fileSEXP(fileID, pool);

  auto args = pool.get_args(fileID);

  for (auto &bbcID : args) {
    if (pool[bbcID].tag != IRI_GEN::BBContainer)
      continue;

    BBContainerSEXP container(bbcID, pool);

    std::vector<IRID> localBindingsVec;
    BindingsSEXP bindingsSEXP(container.getArg_Bindings(), pool);
    IRID localBindingsID = bindingsSEXP.getArg_LocalBindings();

    auto &containerBC = iridiumBuildContext[container.getScopeIDX()];

    auto bbs = pool.get_args(container.getArg_BB());
    for (auto &bbID : bbs) {
      BBSEXP bb(bbID, pool);

      auto localScope = bb.getScopeIDX();
      auto parentScope = iridiumBuildContext[localScope]->parent;

      auto stmts = pool.get_args(bbID);
      for (size_t i = 0; i < stmts.size(); i++) {
        IRID stmtID = stmts[i];
        IRI_TAG currTag = pool[stmtID].tag;
        if (currTag == IRI_GEN::JSImplicitBindingDeclaration) {
          JSImplicitBindingDeclarationSEXP ibs(stmtID, pool);

          IRI_FLAG kind;
          if (ibs.hasJSLET()) {
            kind = IRI_GEN::JSLET;
          } else if (ibs.hasJSCONST()) {
            kind = IRI_GEN::JSCONST;
          } else if (ibs.hasJSVAR()) {
            kind = IRI_GEN::JSVAR;
          } else {
            throw std::runtime_error(
                "[Forge]: Invalid kind for an implicit binding");
          }

          auto bindingSEXP = EnvBindingSEXP::create(pool, ibs.getNAME(), false, false, false,
                                 ibs.hasJSLET(), ibs.hasJSCONST(),
                                 ibs.hasJSVAR(), false, containerBC->scopeIDX, -1,
                                 localScope, parentScope, -1);
          localBindingsVec.push_back(bindingSEXP);
        }
      }
    }

    pool.add_args_to_end(localBindingsID, localBindingsVec);
  }
}
} // namespace IRI_CORE_PASSES
