

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#ifdef PRINT_TAINT_TREE
#include <iostream>
#endif
#include <stdexcept>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

static inline bool isGenericCall(CallSiteSEXP &o) {
  if (o.hasImport())
    return false;
  else if (o.hasSuper())
    return false;
  else if (o.hasV8Intrinsic())
    return false;
  else if (o.hasCCall())
    return false;
  else if (o.hasConstructorCall())
    return false;
  else if (o.hasPrivateCall())
    return false;
  else if (o.hasJSDirectEval())
    return false;
  else
    return true;
}

static inline bool isGenericCall(ApplySEXP &o) {
  if (o.hasConstructorCall())
    return false;
  return true;
}

inline void patchNode(IRIContext &ctx, IRID node, double scopeToTaint,
                      double containerScope) {
  auto args = ctx.storage.nodes.get_args(node);

  IRI_STORAGE::StringID EVAL = ctx.storage.strings.intern("eval");

  if (IRI_NODE(ctx, node).tag == IRI_GEN::CallSite) {
    CallSiteSEXP callSite(node, ctx);
    if (isGenericCall(callSite)) {
      if (ctx.storage.nodes.get_args(node).size() == 0) {
        throw std::runtime_error("Call site with no callee!!");
      }
      auto calleeID = ctx.storage.nodes.get_args(node)[0];
      if (IRI_NODE(ctx, calleeID).tag == IRI_GEN::EnvRead) {
        EnvReadSEXP eRead(calleeID, ctx);
        auto calleeBindingID = eRead.getArg_Obj();
        if (IRI_NODE(ctx, calleeBindingID).tag == IRI_GEN::GlobalBinding) {
          GlobalBindingSEXP gBinding(calleeBindingID, ctx);
          if (gBinding.getNAME() == EVAL) {
            if (ctx.iris->isArgInitScope(scopeToTaint)) {
              throw std::runtime_error("IRI build failed ::TODO:: Eval in ArgInitScope");
            }
            // Direct eval enclosed in a prop init scope is unsupported
            if (ctx.iris->isEnclosedInAPropInitScope(scopeToTaint)) {
              throw std::runtime_error("IRI build failed ::TODO:: Eval in arg init scopes is unsupported");
            }
            ctx.iris->addEvalRemoteBindingsToParentClosure(scopeToTaint);
            ctx.iris->registerDirectEval(node, scopeToTaint, containerScope);
            // double evalREFIDX =
            //     ctx.iris->getJSEvalLookupREFIDX(scopeToTaint, containerScope);
            callSite.setJSDirectEval(-3);
          }
        }
      }
    }
  }

  if (IRI_NODE(ctx, node).tag == IRI_GEN::Apply) {
    ApplySEXP callSite(node, ctx);
    if (isGenericCall(callSite)) {
      if (ctx.storage.nodes.get_args(node).size() == 0) {
        throw std::runtime_error("Apply Call site with no callee!!");
      }
      auto calleeID = ctx.storage.nodes.get_args(node)[0];
      if (IRI_NODE(ctx, calleeID).tag == IRI_GEN::EnvRead) {
        EnvReadSEXP eRead(calleeID, ctx);
        auto calleeBindingID = eRead.getArg_Obj();
        if (IRI_NODE(ctx, calleeBindingID).tag == IRI_GEN::GlobalBinding) {
          GlobalBindingSEXP gBinding(calleeBindingID, ctx);
          if (gBinding.getNAME() == EVAL) {
            if (ctx.iris->isArgInitScope(scopeToTaint)) {
              throw std::runtime_error("IRI build failed ::TODO:: Eval in ArgInitScope");
            }
            // Direct eval enclosed in a prop init scope is unsupported
            if (ctx.iris->isEnclosedInAPropInitScope(scopeToTaint)) {
              throw std::runtime_error("IRI build failed ::TODO:: Eval in arg init scopes is unsupported");
            }
            ctx.iris->addEvalRemoteBindingsToParentClosure(scopeToTaint);
            ctx.iris->registerDirectEval(node, scopeToTaint, containerScope);

            // double evalREFIDX =
            //     ctx.iris->getJSEvalLookupREFIDX(scopeToTaint, containerScope);
            callSite.setJSDirectEval(-3);
          }
        }
      }
    }
  }

  for (int i = 0; i < args.size(); i++) {
    patchNode(ctx, args[i], scopeToTaint, containerScope);
  }
}

void _16_MDE(IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID fileSEXP,
             BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);

    auto containerScope = bbc.getScopeIDX();

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);

      auto bbScope = bb.getScopeIDX();

      for (auto [stmtID, _] : bb.stmts()) {
        auto stmtTag = IRI_NODE(ctx, stmtID).tag;
        patchNode(ctx, stmtID, bbScope, containerScope);
      }
    }
  }

#ifdef PRINT_TAINT_TREE
  std::cout << "Scope tree after eval tainting (†)" << std::endl;
  ctx.iris->dumpFlat(std::cout);
#endif
}
} // namespace IRI_CORE_PASSES
