#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline void patchNode(FileSupport file, IRIContext &ctx, IRID node, double startScopeIDX) {
  auto args = ctx.storage.nodes.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    auto currTag = IRI_NODE(ctx, currID).tag;

    if (currTag == IRI_GEN::ResolveEnvBinding) {
      ResolveEnvBindingSEXP uBinding(currID, ctx);
      auto resolvedbiii = ctx.iris->resolve(uBinding.getNAME(), startScopeIDX);
      ctx.storage.nodes.update_arg_inplace(node, i, resolvedbiii.ID);
      continue;
    }

    patchNode(file, ctx, args[i], startScopeIDX);
  }
}

void _8_RREBS(IRIContext &ctx, IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {
  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    bool SLOPPY = bbc.hasSTRICT();
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);
      auto bbScopeIDX = bb.getScopeIDX();
      for (auto [stmtID, stmtIDX] : bb.stmts()) {
        IRI_TAG currStmtTag = IRI_NODE(ctx, stmtID).tag;
        if (currStmtTag == IRI_GEN::SiblingSpecialWrite) {
          SiblingSpecialWriteSEXP siblingSpecialWrite(stmtID, ctx);
          ResolveEnvBindingSEXP unresolvedBinding(siblingSpecialWrite.getArg_LValTarget(),
                                     ctx);

          IRID lval = ctx.iris->resolve(
              unresolvedBinding.getNAME(), siblingSpecialWrite.getScopeIDX()).ID;

          IRID rval = siblingSpecialWrite.getArg_RVal();

          if (IRI_NODE(ctx, lval).tag != IRI_GEN::EnvBinding) {
            throw std::runtime_error(
                "Expected SiblingSpecialWrite to resolve to EnvBinding");
          }
          IRID writeStmt = LWriteSEXP::create(ctx, lval, rval, true, false, false);

          ctx.storage.nodes.update_arg_inplace(bbID, stmtIDX, writeStmt);
          patchNode(file, ctx, rval, bbScopeIDX);
        } else if (currStmtTag == IRI_GEN::CompoundAssn) {
         CompoundAssnSEXP compoundAssn(stmtID, ctx);
         auto args = ctx.storage.nodes.get_args(stmtID);
         assert(args.size() > 1);
         IRID rval = args[0];
         for (size_t i = 1; i < args.size(); i++) {
           IRID currWriteID = args[i];
           assert(IRI_NODE(ctx, currWriteID).tag == IRI_GEN::EnvWrite);

           EnvWriteSEXP ew(currWriteID, ctx);
           ResolveEnvBindingSEXP unresolvedBinding(ew.getArg_LValTarget(), ctx);

           auto & resolved =  ctx.iris->resolve(unresolvedBinding.getNAME(), bbScopeIDX);
           IRID lval = resolved.ID;
           IRID rval = ew.getArg_RVal();
           bool hasASW = resolved.isARGX;
           IRID target;
           if (IRI_NODE(ctx, lval).tag == IRI_GEN::GlobalBinding || IRI_NODE(ctx, lval).tag == IRI_GEN::ScriptBinding) {
             assert(ew.getTHISINIT() == false);
             target = GWriteSEXP::create(ctx, lval, rval, ew.getCINIT(), ew.getSAFE(), false, false);
             // target = GWriteSEXP::create(ctx, lval, rval, ew.getSAFE(), false, false, false);
           } else if (IRI_NODE(ctx, lval).tag == IRI_GEN::EnvBinding) {
             target = LWriteSEXP::create(ctx, lval, rval, ew.getCINIT(), hasASW || ew.getSAFE(), ew.getTHISINIT());
             // target = LWriteSEXP::create(ctx, lval, rval, hasASW || ew.getSAFE(), false, ew.getTHISINIT());
           } else if (IRI_NODE(ctx, lval).tag == IRI_GEN::RemoteEnvBinding) {
             RemoteEnvBindingSEXP rb(lval, ctx);
             if (rb.hasMODULE() || rb.hasMODULEI() || rb.hasMODULENSI()) {
               assert(ew.getTHISINIT() == false);
               target = MWriteSEXP::create(ctx, lval, rval, ew.getCINIT(), ew.getSAFE());
               // target = MWriteSEXP::create(ctx, lval, rval, ew.getSAFE(), false);
             } else {
               target = RWriteSEXP::create(ctx, lval, rval, ew.getCINIT(), ew.getSAFE(), ew.getTHISINIT());
               // target = RWriteSEXP::create(ctx, lval, rval, ew.getSAFE(), false, ew.getTHISINIT());
             }
           }
           ctx.storage.nodes.update_arg_inplace(stmtID, i, target);
         }
         patchNode(file, ctx, rval, bbScopeIDX);
       } else if (currStmtTag == IRI_GEN::EnvWrite) {
          EnvWriteSEXP ew(stmtID, ctx);
          ResolveEnvBindingSEXP unresolvedBinding(ew.getArg_LValTarget(), ctx);

          auto & resolved =  ctx.iris->resolve(unresolvedBinding.getNAME(), bbScopeIDX);
          IRID lval = resolved.ID;
          IRID rval = ew.getArg_RVal();
          bool hasASW = resolved.isARGX;
          IRID target;
          if (IRI_NODE(ctx, lval).tag == IRI_GEN::GlobalBinding || IRI_NODE(ctx, lval).tag == IRI_GEN::ScriptBinding) {
            assert(ew.getTHISINIT() == false);
            target = GWriteSEXP::create(ctx, lval, rval, ew.getCINIT(), ew.getSAFE(), false, false);
            // target = GWriteSEXP::create(ctx, lval, rval, ew.getSAFE(), false, false, false);
          } else if (IRI_NODE(ctx, lval).tag == IRI_GEN::EnvBinding) {
            target = LWriteSEXP::create(ctx, lval, rval, ew.getCINIT(), hasASW || ew.getSAFE(), ew.getTHISINIT());
            // target = LWriteSEXP::create(ctx, lval, rval, hasASW || ew.getSAFE(), false, ew.getTHISINIT());
          } else if (IRI_NODE(ctx, lval).tag == IRI_GEN::RemoteEnvBinding) {
            RemoteEnvBindingSEXP rb(lval, ctx);
            if (rb.hasMODULE() || rb.hasMODULEI() || rb.hasMODULENSI()) {
              assert(ew.getTHISINIT() == false);
              target = MWriteSEXP::create(ctx, lval, rval, ew.getCINIT(), ew.getSAFE());
              // target = MWriteSEXP::create(ctx, lval, rval, ew.getSAFE(), false);
            } else {
              target = RWriteSEXP::create(ctx, lval, rval, ew.getCINIT(), ew.getSAFE(), ew.getTHISINIT());
              // target = RWriteSEXP::create(ctx, lval, rval, ew.getSAFE(), false, ew.getTHISINIT());
            }
          }
          ctx.storage.nodes.update_arg_inplace(bbID, stmtIDX, target);
          patchNode(file, ctx, rval, bbScopeIDX);
        } else {
          patchNode(file, ctx, stmtID, bbScopeIDX);
        }
      }
      for (auto [stmtID, stmtIDX] : bb.stmts()) {
        IRI_TAG currStmtTag = IRI_NODE(ctx, stmtID).tag;
        if (currStmtTag == LWrite) {
          LWriteSEXP lw(stmtID, ctx);
          IRID ebID = lw.getArg_LValTarget();
          EnvBindingSEXP eb(ebID, ctx);
          if (IRI_BINDING(ctx.iris, eb.id).tombstone) {
            ctx.storage.nodes.update_arg_inplace(bbID, stmtIDX, ctx.storage.nodes.NOP_SEXP);
          }

        }

      }
    }
  }

  patchNode(file, ctx, file.staticImports(), ctx.iris->getTopLevelScope());
  patchNode(file, ctx, file.staticExports(), ctx.iris->getTopLevelScope());
  patchNode(file, ctx, file.staticStarExports(), ctx.iris->getTopLevelScope());

}
} // namespace IRI_CORE_PASSES
