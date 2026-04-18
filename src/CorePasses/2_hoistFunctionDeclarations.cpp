#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _2_HFD(IridiumPool &pool, IRID sexp, BUILD_CTX &iridiumBuildContext) {
  std::unordered_map<double, std::set<IRID>> toHoist;

  FileSEXP fSEXP(sexp, pool);

  const auto bbs = pool.get_args(sexp);

  for (auto &currBBID : bbs) {
    BBSEXP currBB(currBBID, pool);
    const auto args = pool.get_args(currBBID);
    auto scopeIDX = currBB.getScopeIDX();

    for (size_t idx = 0; const auto &stmtID : args) {
      auto &stmt = pool[stmtID];

      if (stmt.tag == IRI_TAG::JSFuncDecl) {
        toHoist[scopeIDX].insert(stmtID);
        pool.update_arg_inplace(currBBID, idx, pool.NULL_SEXP);
      }

      idx++;
    }
  }

  for (auto &e : toHoist) {
    auto &scope = e.first;
    auto &funDeclarations = e.second;
    auto &buildContext = iridiumBuildContext.at(scope);
    auto &targetBB = buildContext->BB[0];

    pool.add_args_to_beginning(
        targetBB, {funDeclarations.begin(), funDeclarations.end()});
  }
}
} // namespace IRI_CORE_PASSES
