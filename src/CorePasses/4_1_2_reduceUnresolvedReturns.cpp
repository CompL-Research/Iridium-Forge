#include "Config.h"
#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include <cstddef>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

static inline bool hasScopePath(double startScope, double targetScope,
                                BUILD_CTX &iridiumBuildContext) {
  if (startScope == targetScope)
    return true;
  if (startScope == -1)
    return false;
  assert(iridiumBuildContext.find(startScope) != iridiumBuildContext.end());
  std::shared_ptr<IridiumBuildContext> buildContext =
      iridiumBuildContext[startScope];

  return hasScopePath(buildContext->parent, targetScope, iridiumBuildContext);
}

static inline bool needsNipTarget(IridiumPool &pool,
                                  BBContainerSupport &container,
                                  double startScope, double currScope,
                                  BUILD_CTX &iridiumBuildContext) {
  if (currScope == -1)
    return false;
  assert(iridiumBuildContext.find(currScope) != iridiumBuildContext.end());
  std::shared_ptr<IridiumBuildContext> buildContext =
      iridiumBuildContext[currScope];

  if (buildContext->tryContext) {
    BBSEXP bb(container.getBBByIDX(buildContext->tryContext.value().tryIDX),
              pool);
    if (hasScopePath(startScope, bb.getScopeIDX(), iridiumBuildContext)) {
      return true;
    }
  }

  auto startBBID = buildContext->BB[0];
  BBSEXP startBB(startBBID, pool);

  if (startBB.hasClosureBoundary())
    return false;

  return needsNipTarget(pool, container, startScope, buildContext->parent,
                        iridiumBuildContext);
}

void _4_1_2_RUR(IridiumPool &pool, IRID fileID,
                BUILD_CTX &iridiumBuildContext) {
  StringID undefined_str = pool.strings.intern("undefined");
  StringID this_str = pool.strings.intern("this");
  FileSupport fileSupport(fileID, pool);
  for (auto [bbContID, _] : fileSupport.containers(false)) {
    BBContainerSupport container(bbContID, pool);

    bool returnThis = container.getContainerFlagID() == CF_DERIVED_CTR;

    std::vector<IRID> newBBs;

    for (auto [bbID, _] : container.bbs()) {
      BBSupport bb(bbID, pool);

      std::vector<size_t> offsetsToInsertAfter;
      std::vector<std::vector<IRID>> chunks;

      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        IRI_TAG currTag = pool[stmtID].tag;

        if (currTag == IRI_GEN::UnresolvedReturn) {
          IRID patched;
          if (returnThis) {
            patched = ReturnSEXP::create(
                pool, IRI_HELPERS::createUnsafeEnvReadSEXP(pool, this_str),
                false);
          } else {
            patched = ReturnSEXP::create(
                pool, IRI_HELPERS::createUnsafeEnvReadSEXP(pool, undefined_str),
                false);
          }
          pool.update_arg_inplace(bbID, stmtOffset, patched);
        } else if (currTag == IRI_GEN::Return) {
          //
          // Decorate Return statements of derived class constructors
          //
          if (returnThis) {
            ReturnSEXP rSEXP(stmtID, pool);
            IRID retObj = rSEXP.getArg_Obj();
            assert(pool[retObj].tag == IRI_GEN::EnvRead &&
                   "Expected Return (EnvRead)");
            EnvReadSEXP eReadSEXP(retObj, pool);
            IRID readBinding = eReadSEXP.getArg_Obj();
            assert(
                pool[readBinding].tag == IRI_GEN::ResolveEnvBinding &&
                "Expected Returns to point to unresolved binding references!!");
            ResolveEnvBindingSEXP rEnvBindingSEXP(readBinding, pool);
            // return ResolveEnvBinding(undefined) -> return
            // ResolveEnvBinding(this)
            if (rEnvBindingSEXP.getNAME() == undefined_str) {
              pool.update_arg_inplace(
                  bbID, stmtOffset,
                  ReturnSEXP::create(
                      pool,
                      IRI_HELPERS::createUnsafeEnvReadSEXP(pool, this_str),
                      false));
            } else {
              pool.update_arg_inplace(bbID, stmtOffset, NOPSEXP::create(pool));

              offsetsToInsertAfter.push_back(stmtOffset);
              std::vector<IRID> currChunk;

              //
              // iri$temp = DCTRRet(userVal)  # Inplace update
              // IfElseJump(iri$temp) ----> Return this
              //                      |
              //                      |
              //                      |---> Return userVal
              //

              // iri$temp = DCTRRet(userVal)
              StringID iriTemp = pool.getTemp();
              // Nip catch, if we were in a catch context
              if (needsNipTarget(pool, container, bb.getScopeIDX(),
                                 bb.getScopeIDX(), iridiumBuildContext)) {
                IRID stackRetain = StackRetainSEXP::create(pool, 1, 0);
                pool.set_args(stackRetain, { NumberSEXP::create(pool, 0) });
                currChunk.push_back(stackRetain);
                currChunk.push_back(NIPCatchCTXSEXP::create(pool));
              }
              currChunk.push_back(JSExplicitBindingDeclarationNSEXP::create(
                  pool,
                  IRI_HELPERS::createNoASWResolveEnvBindingSEXP(pool, iriTemp),
                  DCTRRetSEXP::create(pool,
                                      IRI_HELPERS::createUnsafeEnvReadSEXP(
                                          pool, rEnvBindingSEXP.getNAME())),
                  true, false, false, false, true, false));



              // IfElseJump(iri$temp) ----> Return this
              //                      |
              //                      |
              //                      |---> Return userVal
              double trueBBIDX = ++pool.lastBBIDX;
              double falseBBIDX = ++pool.lastBBIDX;

              currChunk.push_back(IfElseJumpSEXP::create(
                  pool, IRI_HELPERS::createUnsafeEnvReadSEXP(pool, iriTemp),
                  false, trueBBIDX, falseBBIDX));

              chunks.push_back(currChunk);

              IRID bbTrueID = BBSEXP::create(
                  pool, bb.hasTopLevel(), bb.hasClosureBoundary(),
                  bb.hasLexical(), bb.hasVARBoundary(), bb.hasTryBB(),
                  trueBBIDX, bb.getScopeIDX());
              IRID bbFalseID = BBSEXP::create(
                  pool, bb.hasTopLevel(), bb.hasClosureBoundary(),
                  bb.hasLexical(), bb.hasVARBoundary(), bb.hasTryBB(),
                  falseBBIDX, bb.getScopeIDX());

              newBBs.push_back(bbTrueID);
              newBBs.push_back(bbFalseID);

              pool.set_args(
                  bbTrueID,
                  {ReturnSEXP::create(
                      pool,
                      IRI_HELPERS::createUnsafeEnvReadSEXP(pool, this_str),
                      false)});

              pool.set_args(bbFalseID, {ReturnSEXP::create(
                                           pool,
                                           IRI_HELPERS::createUnsafeEnvReadSEXP(
                                               pool, rEnvBindingSEXP.getNAME()),
                                           false)});
            }
          }
        }
      }

      std::vector<IRID> updatedStmtList =
          BBSupport::insert_chunks_after_given_offsets(
              bb.stmtsVec(), offsetsToInsertAfter, chunks);
      pool.set_args(bbID, updatedStmtList);
    }

    if (newBBs.size() > 0) {
      pool.add_args_to_end(container.getArg_BB(), newBBs);
    }
  }
}
} // namespace IRI_CORE_PASSES
