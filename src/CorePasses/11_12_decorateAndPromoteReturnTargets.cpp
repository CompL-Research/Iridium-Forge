#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <stdexcept>
#include <vector>

#define DEC_LOG(msg)
#define DEC_LOG_IDX(label, idx)

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

static inline std::shared_ptr<IridiumBuildContext> findReturnTarget(
    IRIContext &ctx, double localScope, BUILD_CTX &iridiumBuildContext,
    std::vector<std::variant<LoopConfig, TryContext>> &intermediateContexts) {
  if (localScope == -1)
    throw std::runtime_error("Failed to find return target!!!");
  assert(iridiumBuildContext.find(localScope) != iridiumBuildContext.end());
  std::shared_ptr<IridiumBuildContext> buildContext =
      iridiumBuildContext[localScope];

  if (buildContext->tryContext) {
    intermediateContexts.push_back(buildContext->tryContext.value());
  }

  if (buildContext->loopConfig) {
    intermediateContexts.push_back(buildContext->loopConfig.value());
  }

  auto startBBID = buildContext->BB[0];
  BBSEXP startBB(startBBID, ctx);

  if (startBB.hasTopLevel()) {
    if (intermediateContexts.size() > 0)
      throw std::runtime_error("Top level return not expected to be wrapped "
                               "inside intermediate contexts");
    if (buildContext->isModule) {
      throw std::runtime_error(
          "Expected async returns in module top level code...");
    }
  }

  if (startBB.hasClosureBoundary())
    return buildContext;

  return findReturnTarget(ctx, buildContext->parent, iridiumBuildContext,
                          intermediateContexts);
}

void _11_12_DAPRT(
    IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) {
  FileSupport fileSupport(fileSEXP, ctx);
  for (auto [bbContID, _] : fileSupport.containers()) {
    BBContainerSupport bbc(bbContID, ctx);

    std::vector<IRID> res;

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bbSEXP(bbID, ctx);
      auto bbScopeIDX = bbSEXP.getScopeIDX();

      std::unordered_map<IRID,
                         std::vector<std::variant<LoopConfig, TryContext>>>
          decoratorMap;

      for (auto [stmtID, stmtOffset] : bbSEXP.stmts()) {
        auto stmtTag = IRI_NODE(ctx, stmtID).tag;
        if (stmtTag == IRI_GEN::Return) {
          ReturnSEXP rTarget(stmtID, ctx);
          if (bbSEXP.hasTopLevel())
            continue;

          auto retID = stmtID;
          std::vector<std::variant<LoopConfig, TryContext>>
              intermediateContextHolder;
          auto target = findReturnTarget(ctx, bbScopeIDX, iridiumBuildContext,
                                         intermediateContextHolder);

          // Promote to Async return if the return matches an async context
          if (target->isAsync || target->isGenerator) {
            retID = ReturnAsyncSEXP::create(ctx, rTarget.getArg_Obj());
            ctx.storage.nodes.update_arg_inplace(bbID, stmtOffset, retID);
          }

          decoratorMap[retID] = intermediateContextHolder;
        } else if (stmtTag == IRI_GEN::ReturnAsync) {
          if (bbSEXP.hasTopLevel())
            continue;

          std::vector<std::variant<LoopConfig, TryContext>>
              intermediateContextHolder;
          findReturnTarget(ctx, bbScopeIDX, iridiumBuildContext,
                           intermediateContextHolder);
          decoratorMap[stmtID] = intermediateContextHolder;
        }
      }

      std::vector<IRID> newStmtList = ctx.storage.nodes.get_args(bbID);
      // Decorate emitted targets by handling requirements of the enclosing
      // contexts

      for (auto &e : decoratorMap) {
        auto &element = e.first;
        auto &intermediateContexts = e.second;
        for (auto &intermediateContext : intermediateContexts) {
          if (auto loopConfig = std::get_if<LoopConfig>(&intermediateContext)) {
            DEC_LOG("  LoopConfig("
                    << (loopConfig->kind == LoopConfig::Kind::ForOf
                            ? "ForOf"
                            : "Standard")
                    << ")");
            DEC_LOG_IDX("loopHeadIDX", loopConfig->loopHeadIDX);
            DEC_LOG_IDX("loopBodyIDX", loopConfig->loopBodyIDX);
            DEC_LOG_IDX("loopInitIDX", loopConfig->loopInitIDX);

            if (loopConfig->label.has_value()) {
              DEC_LOG("    label: " << loopConfig->label.value());
            } else {
              DEC_LOG("    NO_LABEL: ");
            }
            DEC_LOG_IDX("breakTarget", loopConfig->breakTarget);
            DEC_LOG_IDX("continueTarget", loopConfig->continueTarget);

            if (loopConfig->kind == LoopConfig::Kind::ForOf) {
              IRID forOfIteratorClose = JSForOfIteratorCloseSEXP::create(ctx);
              BBSupport::insert_before(newStmtList, element, forOfIteratorClose);
            }
          } else if (auto tryContext =
                         std::get_if<TryContext>(&intermediateContext)) {
            DEC_LOG("TryContext");
            DEC_LOG_IDX("tryContextIDX", tryContext->tryContextIDX);
            DEC_LOG_IDX("tryIDX", tryContext->tryIDX);
            DEC_LOG_IDX("udCatchIDX", tryContext->udCatchIDX);
            DEC_LOG_IDX("imCatchIDX", tryContext->imCatchIDX);
            DEC_LOG_IDX("finalizerIDX", tryContext->finalizerIDX);

            // If the context is reached via try or catch block, only then pop
            // the catch context and decorate to finalizer (if applicable)
            if (ctx.iris->hasScopePath(
                    bbScopeIDX,
                    BBSupport(bbc.getBBByIDX(tryContext->tryIDX), ctx)
                        .getScopeIDX()) ||
                (tryContext->udCatchIDX > -1 &&
                 ctx.iris->hasScopePath(
                     bbScopeIDX,
                     BBSupport(bbc.getBBByIDX(tryContext->udCatchIDX), ctx)
                         .getScopeIDX()))) {
              BBSupport::insert_before(newStmtList, element,
                                       PopCatchContextSEXP::create(ctx));

              if (tryContext->finalizerIDX > -1) {
                BBSupport::insert_before(newStmtList, element,
                                         InvokeFinalizerSEXP::create(
                                             ctx, tryContext->finalizerIDX));
              }
            } else {
              // This pops the finalizer return target from the stack, should be
              // renamed to prevent confusion
              BBSupport::insert_before(
                  newStmtList, element,
                  PopFinalizerReturnTargetSEXP::create(ctx));
              // This must be reached through a finalizer block, we dont
              // expect any nesting inside the implicit catch block as
              // its outside user
              // interference...
              if (!ctx.iris->hasScopePath(
                      bbSEXP.getScopeIDX(),
                      BBSupport(bbc.getBBByIDX(tryContext->finalizerIDX), ctx)
                          .getScopeIDX())) {
                throw std::runtime_error(
                    "Failed to match finalizer block when matching target!!");
              }
            }
          }
        }
      }

      ctx.storage.nodes.set_args(bbID, newStmtList);
    }
  }
}
} // namespace IRI_CORE_PASSES
