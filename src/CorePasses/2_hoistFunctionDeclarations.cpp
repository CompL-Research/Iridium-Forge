#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IRIContext.h"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _2_HFD(IRIContext &ctx, IRID sexp, BUILD_CTX &iridiumBuildContext) {
  std::unordered_map<double, std::set<IRID>> toHoist;

  FileSEXP fSEXP(sexp, ctx);

  const auto bbs = ctx.storage.nodes.get_args(sexp);

  for (auto &currBBID : bbs) {
    BBSEXP currBB(currBBID, ctx);
    const auto args = ctx.storage.nodes.get_args(currBBID);
    auto scopeIDX = currBB.getScopeIDX();

    for (size_t idx = 0; const auto &stmtID : args) {
      auto &stmt = IRI_NODE(ctx, stmtID);

      if (stmt.tag == IRI_TAG::JSFuncDecl) {
        toHoist[scopeIDX].insert(stmtID);
        ctx.storage.nodes.update_arg_inplace(currBBID, idx, ctx.storage.nodes.NOP_SEXP);
      }

      idx++;
    }
  }

  for (auto &e : toHoist) {
    auto &scope = e.first;
    auto &funDeclarations = e.second;
    auto &buildContext = iridiumBuildContext.at(scope);
    auto &targetBB = buildContext->BB[0];

    ctx.storage.nodes.add_args_to_beginning(
        targetBB, {funDeclarations.begin(), funDeclarations.end()});
  }
}
} // namespace IRI_CORE_PASSES
