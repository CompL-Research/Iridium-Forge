

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/BindingsPool.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
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

inline void patchNode(IRIContext &ctx, IRID node) {
  if (IRI_NODE(ctx, node).tag == IRI_GEN::CallSite) {
    CallSiteSEXP cs(node, ctx);
    if (cs.hasJSDirectEval()) {
      auto args = ctx.storage.nodes.get_args(node);
      for (int i = 1; i < args.size(); i++) {
        patchNode(ctx, args[i]);
      }
      return;
    }
  }

  if (IRI_NODE(ctx, node).tag == IRI_GEN::EnvRead) {
    EnvReadSEXP ev(node, ctx);
    IRID obj = ev.getArg_Obj();
    if (IRI_NODE(ctx, obj).tag == IRI_GEN::RemoteEnvBinding) {
      ev.setTAINTED();
      IRI_NODE(ctx, obj).dumpFlat(std::cerr, &ctx);
      throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable EnvRead: ");
    }

    if (IRI_NODE(ctx, obj).tag == IRI_GEN::GlobalBinding) {
      GlobalBindingSEXP gb(obj, ctx);
      throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable EnvRead: ");
    }

    if (IRI_NODE(ctx, obj).tag == IRI_GEN::ScriptBinding) {
      throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable EnvRead: ");
    }
    return;
  }
  auto args = ctx.storage.nodes.get_args(node);
  for (int i = 0; i < args.size(); i++) {
    patchNode(ctx, args[i]);
  }
}

void _21_TER(IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID fileSEXP,
             BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    if (bbc.hasSTRICT() || ctx.iris->isTopLevelScope(bbc.getScopeIDX()))
      continue;

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);

      if (!ctx.iris->mayReadFromATaintedScope(bb.getScopeIDX())) {
        // std::cout << "mayReadFromATaintedScope fail:" << bb.getScopeIDX() << std::endl;
        continue;
      }

      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        IRI_TAG stmtTAG = IRI_NODE(ctx, stmtID).tag;

        if (stmtTAG == IRI_GEN::GWrite) {
          throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable GWrite");
        }

        if (stmtTAG == IRI_GEN::RWrite) {
          throw std::runtime_error("IRI build failed ::TODO:: Eval Unstable RWrite");
        }

        patchNode(ctx, stmtID);
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
