#include "CorePasses.h"
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
#include <stdexcept>
#include <string>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline void patchNode(IridiumPool &pool, IRID node, double startScopeIDX,
                      BUILD_CTX &iridiumBuildContext) {
  auto args = pool.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    auto currTag = pool[currID].tag;

    if (currTag == IRI_GEN::ResolvePrivateEnvBinding) {
      ResolvePrivateEnvBindingSEXP pvt(currID, pool);
      auto pvtName = pvt.getNAME();
      auto pvtNameStr = std::string(pool.strings.get(pvtName));
      // std::cerr << "ResolvePrivateEnvBinding@" << startScopeIDX << ": " << pvtNameStr << std::endl;
      double targetScopeIDX = startScopeIDX;
      while (true) {
        auto buildContext = iridiumBuildContext[targetScopeIDX];
        // std::cerr << " At Scope: " << targetScopeIDX << std::endl;
        if (buildContext->privateMapping) {
          // std::cerr << "  PVTMapping@" << targetScopeIDX << ": ";
          for (auto &p : buildContext->privateMapping.value()) {
            // std::cerr << "[" << p.first << " -> (" << p.second.first << "," << p.second.second << ")] ";
          }
          // std::cerr << std::endl;
          auto privateMapping = buildContext->privateMapping.value();
          if (privateMapping.find(pvtNameStr) != privateMapping.end()) {
            auto pvtInfo = privateMapping[pvtNameStr];
            pool.update_arg_inplace(
                node, i,
                PVTEnvReadSEXP::create(
                    pool,
                    IRI_HELPERS::createNoASWResolveEnvBindingSEXP(
                        pool, pool.strings.intern(pvtInfo.first)),
                    pvtInfo.second == "PROP", pvtInfo.second == "METHOD",
                    pvt.hasFULLY_RESOLVE()));
            break;
          }
        } else {
          // std::cerr << "  PVTMapping: None" << std::endl;
        }
        // Move to parent lexical scope before finding the aprent closure scope,
        // otherwise we might end up in infinite loops
        targetScopeIDX = IRI_HELPERS::getLexicalScope(pool, targetScopeIDX,
                                                      iridiumBuildContext);
        // targetScopeIDX = IRI_HELPERS::findParentClosureScope(
        //     pool, targetScopeIDX, iridiumBuildContext);
        if (targetScopeIDX < 0)
          throw std::runtime_error("Failed to resolve private binding : " +
                                   pvtNameStr);
      }
    }
  }

  for (int i = 0; i < args.size(); i++)
    patchNode(pool, args[i], startScopeIDX, iridiumBuildContext);
}

void _7_RRPEBS(IridiumPool &pool, IRID fileSEXP,
               BUILD_CTX &iridiumBuildContext) {
  // std::cerr << "::PVT Mapping::" << std::endl;
  for (auto &e : iridiumBuildContext) {
    if (e.second->privateMapping) {
      // std::cerr << "  PVTMapping@" << e.first << ": ";
      for (auto &p : e.second->privateMapping.value()) {
        // std::cerr << "[" << p.first << " -> (" << p.second.first << "," << p.second.second << ")] ";
      }
      // std::cerr << std::endl;
    }
  }

  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);

    auto currentScopeIDX = bbc.getScopeIDX();

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);
      for (auto [stmtID, _] : bb.stmts()) {
        patchNode(pool, stmtID, bb.getScopeIDX(), iridiumBuildContext);
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
