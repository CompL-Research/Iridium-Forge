
#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/IRIS.hpp"
#include "Support/FileSupport.hpp"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline void patchNode(FileSupport file, IRIContext &ctx, IRID node,
                      double startScopeIDX, BUILD_CTX &iridiumBuildContext,
                      size_t &unresolvedReferences, std::vector<IRID> &res) {
  if (unresolvedReferences == 0)
    return;
  auto args = ctx.storage.nodes.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    auto currTag = IRI_NODE(ctx, currID).tag;

    if (currTag == IRI_GEN::Lambda) {
      IRID pb =
          PoolBindingSEXP::create(ctx, currID, -1);
      res.push_back(pb);
      ctx.storage.nodes.update_arg_inplace(node, i, pb);
      unresolvedReferences--;
      continue;
    }

    if (unresolvedReferences == 0)
      return;

    patchNode(file, ctx, args[i], startScopeIDX, iridiumBuildContext,
              unresolvedReferences, res);
  }
}

void _9_RLT(IRIContext &ctx, IRID fileSEXP, BUILD_CTX &iridiumBuildContext) {
  FileSupport fileSupport(fileSEXP, ctx);
  for (auto [bbContID, _] : fileSupport.containers()) {
    BBContainerSupport container(bbContID, ctx);
    double scopeIDX = container.getScopeIDX();
    std::vector<IRID> res;

    for (auto [bbID, _] : container.bbs()) {
      BBSupport bb(bbID, ctx);
      auto bbScopeIDX = bb.getScopeIDX();

      for (auto [stmtID, _] : bb.stmts()) {
        size_t unresolvedReferences = 0;
        IRI_HELPERS::countNodeOccurenceWithPredicate(
            stmtID, &ctx,
            [&](IRID arg) { return IRI_NODE(ctx, arg).tag == IRI_GEN::Lambda; },
            unresolvedReferences);
        if (unresolvedReferences > 0) {
          patchNode(fileSupport, ctx, stmtID, bbScopeIDX, iridiumBuildContext,
                    unresolvedReferences, res);
          assert(unresolvedReferences == 0);
        }
      }
    }

    for (size_t i = 0; i < res.size(); i++) {
      PoolBindingSEXP pbSEXP(res[i], ctx);
      ctx.iris->addClosureAtScope(scopeIDX, res[i]);
    }
  }
}
} // namespace IRI_CORE_PASSES
