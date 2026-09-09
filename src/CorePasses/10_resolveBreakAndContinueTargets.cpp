#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <stdexcept>
#include <vector>

#ifdef DEBUG_DECORATOR_PASS
#include <iostream>

// General logging macro
#define DEC_LOG(msg) std::cout << msg << std::endl

// Helper macro to conditionally print the @ scope if the index is valid
#define DEC_LOG_IDX(label, idx)                                                \
  do {                                                                         \
    if ((idx) > -1) {                                                          \
      std::cout << "    " << label << ": " << (idx) << "@"                     \
                << BBSupport(bbc.getBBByIDX(idx), pool).getScopeIDX()          \
                << std::endl;                                                  \
    } else {                                                                   \
      std::cout << "    " << label << ": " << (idx) << std::endl;              \
    }                                                                          \
  } while (0)
#else
#define DEC_LOG(msg)
#define DEC_LOG_IDX(label, idx)
#endif

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

static inline LoopConfig findLoopControlTarget(
    IridiumPool &pool, double localScope,
    std::variant<ResolveBreakTargetSEXP, ResolveContinueTargetSEXP> node,
    BUILD_CTX &iridiumBuildContext,
    std::vector<std::variant<LoopConfig, TryContext>> &intermediateContexts) {
  if (localScope == -1)
    throw std::runtime_error("Failed to find loop control target!!!");
  assert(iridiumBuildContext.contains(localScope));

  auto &buildContext = iridiumBuildContext[localScope];

  if (buildContext->tryContext) {
    intermediateContexts.push_back(buildContext->tryContext.value());
  }

  // If not loop context, recurse
  if (!buildContext->loopConfig)
    return findLoopControlTarget(pool, buildContext->parent, node,
                                 iridiumBuildContext, intermediateContexts);

  auto &loopConfig = buildContext->loopConfig;

  bool hasLabel = false;
  std::string label;

  if (auto breakTarget = std::get_if<ResolveBreakTargetSEXP>(&node)) {
    hasLabel = breakTarget->hasLabel();
    if (hasLabel)
      label = pool.strings.get(breakTarget->getLabel());
  } else if (auto continueTarget =
                 std::get_if<ResolveContinueTargetSEXP>(&node)) {
    hasLabel = continueTarget->hasLabel();
    if (hasLabel)
      label = pool.strings.get(continueTarget->getLabel());
  }

  // Intermediate loop context, but not the one we are trying to flow to
  if (hasLabel) {
    if ((!loopConfig->label) || (label != loopConfig->label.value())) {
      intermediateContexts.push_back(loopConfig.value());
      return findLoopControlTarget(pool, buildContext->parent, node,
                                   iridiumBuildContext, intermediateContexts);
    }
  }

  // If we are looking for a continue target, but the resolved target does not
  // have a continue target, keep looking...
  if (std::holds_alternative<ResolveContinueTargetSEXP>(node) &&
      loopConfig->continueTarget == -1) {
    intermediateContexts.push_back(loopConfig.value());
    return findLoopControlTarget(pool, buildContext->parent, node,
                                 iridiumBuildContext, intermediateContexts);
  }

  return loopConfig.value();
}

void _10_RBACT(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) {

#ifdef DEBUG_DECORATOR_PASS
  std::cout << "::_10_RBACT::" << std::endl;
  pool.iris->dumpFlat(std::cout);
#endif
  FileSupport fileSupport(fileSEXP, pool);
  for (auto [bbContID, _] : fileSupport.containers()) {
    BBContainerSupport bbc(bbContID, pool);

    std::vector<IRID> res;

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bbSEXP(bbID, pool);
      auto bbScopeIDX = bbSEXP.getScopeIDX();

      std::unordered_map<IRID,
                         std::vector<std::variant<LoopConfig, TryContext>>>
          decoratorMap;
      std::unordered_map<IRID, LoopConfig> isBreakTarget;

      for (auto [stmtID, stmtOffset] : bbSEXP.stmts()) {
        auto stmtTag = pool[stmtID].tag;
        if (stmtTag == IRI_GEN::ResolveBreakTarget) {
          ResolveBreakTargetSEXP bTarget(stmtID, pool);

          std::vector<std::variant<LoopConfig, TryContext>>
              intermediateContextHolder;

          LoopConfig target = findLoopControlTarget(pool, bbScopeIDX, bTarget,
                                                    iridiumBuildContext,
                                                    intermediateContextHolder);
          auto gotoSEXPID = GotoSEXP::create(pool, false, target.breakTarget);
          pool.update_arg_inplace(bbID, stmtOffset, gotoSEXPID);
          decoratorMap[gotoSEXPID] = intermediateContextHolder;
          isBreakTarget[gotoSEXPID] = target;
        } else if (stmtTag == IRI_GEN::ResolveContinueTarget) {
          ResolveContinueTargetSEXP cTarget(stmtID, pool);

          std::vector<std::variant<LoopConfig, TryContext>>
              intermediateContextHolder;
          LoopConfig target = findLoopControlTarget(pool, bbScopeIDX, cTarget,
                                                    iridiumBuildContext,
                                                    intermediateContextHolder);

          // auto target = findLoopControlTarget(bbSEXP.getScopeIDX(), cTarget,
          //                                     iridiumBuildContext,
          //                                     intermediateContextHolder);
          auto gotoSEXPID = GotoSEXP::create(pool, false, target.continueTarget);
          GotoSEXP gotoSEXP(gotoSEXPID, pool);

          // Used to find stack->heap movement targets (breakTarget becomes
          // intermediate CTX)
          pool.CONTINUE_TARGETS[gotoSEXPID] = target.breakTarget;

          pool.update_arg_inplace(bbID, stmtOffset, gotoSEXPID);

          decoratorMap[gotoSEXPID] = intermediateContextHolder;
        }
      }

      std::vector<IRID> newStmtList = pool.get_args(bbID);
      // Decorate emitted targets by handling requirements of the enclosing
      // contexts
      for (auto &e : decoratorMap) {

        auto &element = e.first;
        auto &intermediateContexts = e.second;

#ifdef DEBUG_DECORATOR_PASS
        if (!intermediateContexts.empty()) {
          std::cout << "::Decorator Map::\n"
                    << "BB: " << bbSEXP.getIDX() << "@" << bbScopeIDX << "\n"
                    << "Stmt: \n";
          pool[element].dumpFlat(std::cout, &pool, 2);
          std::cout << "\nIntermediate Contexts: \n";
        }
#endif

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
              IRID forOfIteratorClose = JSForOfIteratorCloseSEXP::create(pool);
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
            if (pool.iris->hasScopePath(
                    bbSEXP.getScopeIDX(),
                    BBSupport(bbc.getBBByIDX(tryContext->tryIDX), pool)
                        .getScopeIDX()) ||
                (tryContext->udCatchIDX > -1 &&
                 pool.iris->hasScopePath(
                     bbSEXP.getScopeIDX(),
                     BBSupport(bbc.getBBByIDX(tryContext->udCatchIDX), pool)
                         .getScopeIDX()))) {

              BBSupport::insert_before(newStmtList, element,
                                       PopCatchContextSEXP::create(pool));

              if (tryContext->finalizerIDX > -1) {
                BBSupport::insert_before(newStmtList, element,
                                         InvokeFinalizerSEXP::create(
                                             pool, tryContext->finalizerIDX));
              }
            } else {

              // This pops the finalizer return target from the stack, should be
              // renamed to prevent confusion
              BBSupport::insert_before(
                  newStmtList, element,
                  PopFinalizerReturnTargetSEXP::create(pool));

              // This must be reached through a finalizer block, we dont
              // expect any nesting inside the implicit catch block as
              // its outside user
              // interference...
              if (!pool.iris->hasScopePath(
                      bbSEXP.getScopeIDX(),
                      BBSupport(bbc.getBBByIDX(tryContext->finalizerIDX), pool)
                          .getScopeIDX())) {
                throw std::runtime_error(
                    "Failed to match finalizer block when matching target!!");
              }
            }
          }
        }

        if (isBreakTarget.contains(element)) {
          auto &finalLoopConfig = isBreakTarget[element];
          if (finalLoopConfig.kind == LoopConfig::Kind::ForOf) {
            auto forOfIteratorClose = JSForOfIteratorCloseSEXP::create(pool);
            BBSupport::insert_before(newStmtList, element, forOfIteratorClose);
          }
        }
      }

      pool.set_args(bbID, newStmtList);
    }
  }
}
} // namespace IRI_CORE_PASSES
