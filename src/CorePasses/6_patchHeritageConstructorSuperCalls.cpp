#include "CorePasses.h"
#include "Config.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
#include "Storage/StringPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"
#include <functional>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;


inline std::vector<IRID> heritageThisInit(IridiumPool & pool, StringID thisValHolder, StringID propInitClos)
{
  std::vector<IRID> res;

  res.push_back(
    EnvWriteSEXP::create(pool,
      IRI_HELPERS::createNoASWResolveEnvBindingSEXP(pool, pool.strings.intern("this")),
      IRI_HELPERS::createUnsafeEnvReadSEXP(pool, thisValHolder),
      false,
      false,
      false,
      true // <- This is about the only place where we set THISINIT flag to true
    )
  );

  IRID callSite = CallSiteSEXP::create(pool, true, false, false, false, false, false, false, 0);
  CallSiteSEXP callSEXP(callSite, pool);
  callSEXP.clearJSDirectEval();

  std::vector<IRID> callSiteArgs;
  callSiteArgs.push_back(
    IRI_HELPERS::createUnsafeEnvReadSEXP(pool, pool.strings.intern("this"))
  );

  callSiteArgs.push_back(
    IRI_HELPERS::createUnsafeEnvReadSEXP(pool, propInitClos)
  );
  pool.set_args(callSite, callSiteArgs);

  IRID stackRej = StackRejectSEXP::create(pool, 1);
  pool.set_args(stackRej, {callSite});

  res.push_back(stackRej);
  return res;
}

void _6_PHCSC(IridiumPool &pool, IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {

  std::function<bool(IRID)> pred = [&](IRID id) {
    if (pool[id].tag == IRI_GEN::CallSite) {
      CallSiteSEXP cs(id, pool);
      return cs.hasSuper();
    }
    if (pool[id].tag == IRI_GEN::Apply) {
      ApplySEXP cs(id, pool);
      return cs.hasSuper();
    }
    return false;
  };
  FileSupport fileSupport(fileSEXP, pool);
  for (auto [bbContID, _] : fileSupport.containers()) {
    BBContainerSupport container(bbContID, pool);

    for (auto [bbID, _] : container.bbs()) {
      BBSupport bb(bbID, pool);
      std::vector<IRID> origStmtsListCopy = bb.stmtsVec();
      std::vector<IRID> stmtsContainingSuperCall;
      std::vector<size_t> stmtOffsetsContainingSuperCall;
      std::vector<std::vector<IRID>> chunks;

      // Find stmts that contain super calls
      for (auto [stmtID, offset] : bb.stmts()) {
        if (IRI_HELPERS::hasNodeWithPredicate(stmtID, &pool, pred)) {
          stmtsContainingSuperCall.push_back(stmtID);
          stmtOffsetsContainingSuperCall.push_back(offset);
        }
      }

      if (stmtsContainingSuperCall.empty()) continue;

      // Compute chunks to be inserted at after each call
      auto &closureScope = iridiumBuildContext[bb.getScopeIDX()];
      int i = 0;
      for (auto &scallHolder : stmtsContainingSuperCall) {
        i++;

        // Walk up context chain
        auto buildContext = closureScope;

        while (true) {
          if (buildContext->kind == getDerivedConstructorClosureFlag()) {
            EnvWriteSEXP envWrite(scallHolder, pool);

            auto lValHolder = envWrite.getArg_LValTarget();
            ResolveEnvBindingSEXP resolveEnv(lValHolder, pool);

            if (!buildContext->propInitClos)
              throw std::runtime_error("Constructors with heritage are "
                                       "expected to have propInitClos");

            // Computed chunk
            chunks.push_back(heritageThisInit(pool, resolveEnv.getNAME(), pool.strings.intern(buildContext->propInitClos.value())));
            break;
          } else {
            if (iridiumBuildContext.find(buildContext->parent) !=
                iridiumBuildContext.end()) {
              buildContext = iridiumBuildContext[buildContext->parent];
            } else {
              throw std::runtime_error(
                  "buildContext is undefined, failed to patch super!");
            }
          }
        }
      }

      std::vector<IRID> updatedStmtList = BBSupport::insert_chunks_after_given_offsets(origStmtsListCopy, stmtOffsetsContainingSuperCall, chunks);

      pool.set_args(bbID, updatedStmtList);
    }
  }
}
} // namespace IRI_CORE_PASSES
