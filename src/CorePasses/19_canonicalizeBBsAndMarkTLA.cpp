

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <stdexcept>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline static bool isCFlowStmt(IRI_TAG tag) {
  if (ReturnAsync == tag)
    return true;
  if (Goto == tag)
    return true;
  if (IfElseJump == tag)
    return true;
  if (Ret == tag)
    return true;
  if (Return == tag)
    return true;
  if (Throw == tag)
    return true;
  return false;
}

inline static size_t CBB(IRI_STORAGE::IRIContext &ctx, std::vector<IRID> &v) {
  if (v.size() == 0)
    throw std::runtime_error("Found a BB with zero statements, unexpected");

  for (size_t i = 0; i < v.size(); i++) {
    if (isCFlowStmt(IRI_NODE(ctx, v[i]).tag))
      return i + 1;
  }
  throw std::runtime_error("Found a BB with no control flow statement");
}

void _19_CBBAMTLA(IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID fileSEXP,
             BUILD_CTX &iridiumBuildContext) {
  IRID topLevelContainer = IRI_HELPERS::getTopLevelContainer(ctx, fileSEXP);
  FileSupport file(fileSEXP, ctx);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, ctx);
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, ctx);
      std::vector<IRID> newBB = ctx.storage.nodes.get_args(bbID);
      //
      // Mark TLA before it is possibly deleted
      //
      if (bbcID == topLevelContainer) {
        for (auto [stmtID, _] : bb.stmts()) {
          auto currTag = IRI_NODE(ctx, stmtID).tag;
          if (IRI_HELPERS::hasNodeWithPredicate(stmtID, &ctx, [&](IRID id) { return IRI_NODE(ctx, id).tag == IRI_GEN::Await; })) {
            file.setTLA();
            break;
          }
        }
      }

        ctx.storage.nodes.update_num_args(bbID, CBB(ctx, newBB));
    }
  }
}
} // namespace IRI_CORE_PASSES
