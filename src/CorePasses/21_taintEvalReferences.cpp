

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/BindingsPool.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <memory>
#include <stdexcept>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline void patchNode(IridiumPool &pool, IRID node) {
  if (pool[node].tag == IRI_GEN::CallSite) {
    CallSiteSEXP cs(node, pool);
    if (cs.hasJSDirectEval()) {
      auto args = pool.get_args(node);
      for (int i = 1; i < args.size(); i++) {
        patchNode(pool, args[i]);
      }
      return;
    }
  }

  if (pool[node].tag == IRI_GEN::EnvRead) {
    EnvReadSEXP ev(node, pool);
    IRID obj = ev.getArg_Obj();
    if (pool[obj].tag == IRI_GEN::RemoteEnvBinding) {
      ev.setTAINTED();
      pool[obj].dumpFlat(std::cerr, &pool);
      throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable EnvRead: ");
    }

    if (pool[obj].tag == IRI_GEN::GlobalBinding) {
      GlobalBindingSEXP gb(obj, pool);
      throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable EnvRead: ");
    }

    if (pool[obj].tag == IRI_GEN::ScriptBinding) {
      throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable EnvRead: ");
    }
    return;
  }
  auto args = pool.get_args(node);
  for (int i = 0; i < args.size(); i++) {
    patchNode(pool, args[i]);
  }
}

void _21_TER(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
             BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);
    if (bbc.hasSTRICT() || pool.iris->isTopLevelScope(bbc.getScopeIDX()))
      continue;

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);

      if (!pool.iris->mayReadFromATaintedScope(bb.getScopeIDX())) {
        // std::cout << "mayReadFromATaintedScope fail:" << bb.getScopeIDX() << std::endl;
        continue;
      }

      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        IRI_TAG stmtTAG = pool[stmtID].tag;

        if (stmtTAG == IRI_GEN::GWrite) {
          throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable GWrite");
        }

        if (stmtTAG == IRI_GEN::RWrite) {
          throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable RWrite");
        }

        patchNode(pool, stmtID);
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
