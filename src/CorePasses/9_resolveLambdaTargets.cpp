
#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline void patchNode(FileSupport file, IridiumPool &pool, IRID node,
                      double startScopeIDX, BUILD_CTX &iridiumBuildContext,
                      size_t &unresolvedReferences, std::vector<IRID> &res) {
  if (unresolvedReferences == 0)
    return;
  auto args = pool.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    auto currTag = pool[currID].tag;

    if (currTag == IRI_GEN::Lambda) {
      LambdaSEXP lSexp(currID, pool);
      IRID pb =
          PoolBindingSEXP::create(pool, currID, lSexp.getStartBBIDX(), -1);
      res.push_back(pb);
      pool.update_arg_inplace(node, i, pb);
      unresolvedReferences--;
      continue;
    }

    if (unresolvedReferences == 0)
      return;

    patchNode(file, pool, args[i], startScopeIDX, iridiumBuildContext,
              unresolvedReferences, res);
  }
}

void _9_RLT(IridiumPool &pool, IRID fileSEXP, BUILD_CTX &iridiumBuildContext) {
  FileSupport fileSupport(fileSEXP, pool);
  for (auto [bbContID, _] : fileSupport.containers()) {
    BBContainerSupport container(bbContID, pool);

    std::vector<IRID> res;

    for (auto [bbID, _] : container.bbs()) {
      BBSupport bb(bbID, pool);
      auto bbScopeIDX = bb.getScopeIDX();

      for (auto [stmtID, _] : bb.stmts()) {
        size_t unresolvedReferences = 0;
        IRI_HELPERS::countNodeOccurenceWithPredicate(
            stmtID, &pool,
            [&](IRID arg) { return pool[arg].tag == IRI_GEN::Lambda; },
            unresolvedReferences);
        if (unresolvedReferences > 0) {
          patchNode(fileSupport, pool, stmtID, bbScopeIDX, iridiumBuildContext,
                    unresolvedReferences, res);
          assert(unresolvedReferences == 0);
        }
      }
    }

    BindingsSupport bindings(container.getArg_Bindings(), pool);
    ListSEXP lambdas(bindings.getArg_Lambdas(), pool);
    size_t oldNumLambdas = pool.get_args_view(lambdas.id).size();
    for (size_t i = 0; i < res.size(); i++) {
      PoolBindingSEXP pbSEXP(res[i], pool);
      pbSEXP.setREFIDX(oldNumLambdas + i);
    }
    pool.add_args_to_end(lambdas.id, res);
  }
}
} // namespace IRI_CORE_PASSES
