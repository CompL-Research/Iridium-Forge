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
#include <stdexcept>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

// A method call o.eval(x) reads its callee into a temp first, so only a plain
// eval(x) has the global eval binding as its callee.
static bool isGlobalEvalRead(IRIContext &ctx, IRID callee) {
  if (IRI_NODE(ctx, callee).tag != IRI_GEN::EnvRead)
    return false;
  IRID binding = EnvReadSEXP(callee, ctx).getArg_Obj();
  return IRI_NODE(ctx, binding).tag == IRI_GEN::GlobalBinding &&
         GlobalBindingSEXP(binding, ctx).getNAME() ==
             ctx.storage.strings.intern("eval");
}

// Replaces a direct eval call with a JSEval node:
//   CallSite(eval, this, ...args) -> JSEval(eval, ...args)
//   Apply(eval, this, argList)    -> JSEvalApply(eval, argList)
// Returns the node to use in place of `node`; the caller stores it.
inline IRID patchNode(IRIContext &ctx, IRID node, double scopeToTaint,
                      double containerScope) {
  auto tag = IRI_NODE(ctx, node).tag;
  if ((tag == IRI_GEN::CallSite || tag == IRI_GEN::Apply) &&
      isGlobalEvalRead(ctx, ctx.storage.nodes.get_args(node)[0])) {
    if (ctx.iris->isArgInitScope(scopeToTaint)) {
      throw std::runtime_error("IRI build failed ::TODO:: Eval in ArgInitScope");
    }
    // Direct eval enclosed in a prop init scope is unsupported
    if (ctx.iris->isEnclosedInAPropInitScope(scopeToTaint)) {
      throw std::runtime_error("IRI build failed ::TODO:: Eval in arg init scopes is unsupported");
    }
    ctx.iris->addEvalRemoteBindingsToParentClosure(scopeToTaint);

    auto callArgs = ctx.storage.nodes.get_args(node);
    std::vector<IRID> evalArgs = {callArgs[0]};
    for (size_t i = 2; i < callArgs.size(); i++)
      evalArgs.push_back(callArgs[i]);
    // ScopeIDX is resolved later by IRIS
    IRID evalNode = tag == IRI_GEN::CallSite
                        ? JSEvalSEXP::create(ctx, callArgs[0], -3)
                        : JSEvalApplySEXP::create(ctx, callArgs[0], callArgs[2], -3);
    ctx.storage.nodes.set_args(evalNode, evalArgs);
    ctx.iris->registerDirectEval(evalNode, scopeToTaint, containerScope);
    node = evalNode;
  }

  auto args = ctx.storage.nodes.get_args(node);
  for (size_t i = 0; i < args.size(); i++) {
    IRID patched = patchNode(ctx, args[i], scopeToTaint, containerScope);
    if (patched != args[i])
      ctx.storage.nodes.update_arg_inplace(node, i, patched);
  }
  return node;
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

      for (auto [stmtID, offset] : bb.stmts()) {
        IRID patched = patchNode(ctx, stmtID, bbScope, containerScope);
        if (patched != stmtID)
          ctx.storage.nodes.update_arg_inplace(bbID, offset, patched);
      }
    }
  }

}
} // namespace IRI_CORE_PASSES
