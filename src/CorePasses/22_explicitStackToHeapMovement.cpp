

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

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

void _22_ESTKTHM(IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID fileSEXP,
                 BUILD_CTX &iridiumBuildContext,
                 std::unordered_map<IRID, double> CONTINUE_TARGETS) {

  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);
      std::vector<IRID> newBB = ctx.storage.nodes.get_args(bbID);
      size_t oldNumArgs = newBB.size();

      double currScope = bb.getScopeIDX();
      bool dirty = false;
      std::vector<IRID> &args = newBB;
      for (auto it = args.begin(); it != args.end(); ++it) {
        IRID stmtID = *it;
        IRI_TAG stmtTAG = IRI_NODE(ctx, stmtID).tag;

        if (stmtTAG == IRI_GEN::Goto) {
          GotoSEXP gotoSEXP(stmtID, ctx);
          bool isContinueCTX = CONTINUE_TARGETS.contains(stmtID);
          double targetScope;

          if (isContinueCTX) {
            targetScope =
                BBSupport(bbc.getBBByIDX(CONTINUE_TARGETS[stmtID]), ctx)
                    .getScopeIDX();
            CONTINUE_TARGETS.erase(stmtID);
          } else {
            targetScope =
                BBSupport(bbc.getBBByIDX(gotoSEXP.getIDX()), ctx).getScopeIDX();
          }

          auto bindingsToMove =
              ctx.iris->getBindingsToMoveToHeap(currScope, targetScope);

          if (bindingsToMove.size() > 0) {
            dirty = true;
            IRI_STORAGE::IRID stackToHeapNodeID = StackToHeapSEXP::create(ctx);
            ctx.storage.nodes.set_args(stackToHeapNodeID, bindingsToMove);
            // Insert stackToHeapNode just before the current stmt in the
            // vector...
            it = args.insert(it, stackToHeapNodeID);
            // Advance 'it' to point back to the original 'goto' statement
            // so the next loop increment moves us to the statement after 'goto'
            ++it;
          }

        } else if (stmtTAG == IRI_GEN::Throw) {
          double targetScope = ctx.iris->getEnclosingThrowScope(currScope);
          auto bindingsToMove =
              ctx.iris->getBindingsToMoveToHeap(currScope, targetScope);

          if (bindingsToMove.size() > 0) {
            dirty = true;
            IRI_STORAGE::IRID stackToHeapNodeID = StackToHeapSEXP::create(ctx);
            ctx.storage.nodes.set_args(stackToHeapNodeID, bindingsToMove);
            // Insert stackToHeapNode just before the current stmt in the
            // vector...
            it = args.insert(it, stackToHeapNodeID);
            // Advance 'it' to point back to the original 'goto' statement
            // so the next loop increment moves us to the statement after 'goto'
            ++it;
          }

        } else if (stmtTAG == IRI_GEN::LoopInitPreludeEnd) {

          dirty = true;

          double targetScope = currScope;
          auto bindingsToMove =
              ctx.iris->getBindingsToMoveToHeap(currScope, targetScope);

          // This statement is transitional, marks the end of a loop
          // at this point the scope must release all escaping bindings in the
          // top level loop scope (if any).
          if (bindingsToMove.size() > 0) {
            IRI_STORAGE::IRID stackToHeapNodeID = StackToHeapSEXP::create(ctx);
            ctx.storage.nodes.set_args(stackToHeapNodeID, bindingsToMove);
            *it = stackToHeapNodeID;
          } else {

            *it = ctx.storage.nodes.NOP_SEXP;
          }
        }
      }

      if (dirty) {
        ctx.storage.nodes.set_args(bbID, newBB);
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
