#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IRIContext.h"
#include "Storage/StringPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/IRIS.hpp"
#include "Support/FileSupport.hpp"
#include <stdexcept>
#include <string>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline void patchNode(IRIContext &ctx, IRID node, double startScopeIDX,
                      BUILD_CTX &iridiumBuildContext) {
  auto args = ctx.storage.nodes.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    auto currTag = IRI_NODE(ctx, currID).tag;

    if (currTag == IRI_GEN::ResolvePrivateEnvBinding) {
      ResolvePrivateEnvBindingSEXP pvt(currID, ctx);
      auto pvtName = pvt.getNAME();
      auto pvtNameStr = std::string(ctx.storage.strings.get(pvtName));
      double targetScopeIDX = startScopeIDX;
      while (true) {
        auto buildContext = iridiumBuildContext[targetScopeIDX];
        if (buildContext->privateMapping) {
          auto privateMapping = buildContext->privateMapping.value();
          if (privateMapping.find(pvtNameStr) != privateMapping.end()) {
            auto pvtInfo = privateMapping[pvtNameStr];
            ctx.storage.nodes.update_arg_inplace(
                node, i,
                PVTEnvReadSEXP::create(
                    ctx,
                    IRI_HELPERS::createNoASWResolveEnvBindingSEXP(
                        ctx, ctx.storage.strings.intern(pvtInfo.first)),
                    pvtInfo.second == "PROP", pvtInfo.second == "METHOD",
                    pvt.hasFULLY_RESOLVE()));
            break;
          }
        }
        // Move to parent lexical scope before finding the aprent closure scope,
        // otherwise we might end up in infinite loops
        targetScopeIDX = ctx.iris->getLexicalScope(targetScopeIDX);
        if (targetScopeIDX < 0)
          throw std::runtime_error("Failed to resolve private binding : " +
                                   pvtNameStr);
      }
    }
  }

  for (int i = 0; i < args.size(); i++)
    patchNode(ctx, args[i], startScopeIDX, iridiumBuildContext);
}

void _7_RRPEBS(IRIContext &ctx, IRID fileSEXP,
               BUILD_CTX &iridiumBuildContext) {
  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);

    auto currentScopeIDX = bbc.getScopeIDX();

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);
      for (auto [stmtID, _] : bb.stmts()) {
        patchNode(ctx, stmtID, bb.getScopeIDX(), iridiumBuildContext);
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
