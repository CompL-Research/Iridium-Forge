#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/IridiumPool.h"
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

inline void patchNode(FileSupport file, IridiumPool &pool, IRID node, double startScopeIDX) {
  auto args = pool.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    auto currTag = pool[currID].tag;

    if (currTag == IRI_GEN::ResolveEnvBinding) {
      ResolveEnvBindingSEXP uBinding(currID, pool);
      auto resolvedbiii = pool.iris->resolve(uBinding.getNAME(), startScopeIDX);
      pool.update_arg_inplace(node, i, resolvedbiii.ID);
      continue;
    }

    patchNode(file, pool, args[i], startScopeIDX);
  }
}

void _8_RREBS(IridiumPool &pool, IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {
  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);
    bool SLOPPY = bbc.hasSTRICT();
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);
      auto bbScopeIDX = bb.getScopeIDX();
      for (auto [stmtID, stmtIDX] : bb.stmts()) {
        IRI_TAG currStmtTag = pool[stmtID].tag;
        if (currStmtTag == IRI_GEN::SiblingSpecialWrite) {
          SiblingSpecialWriteSEXP siblingSpecialWrite(stmtID, pool);
          ResolveEnvBindingSEXP unresolvedBinding(siblingSpecialWrite.getArg_LValTarget(),
                                     pool);

          IRID lval = pool.iris->resolve(
              unresolvedBinding.getNAME(), siblingSpecialWrite.getScopeIDX()).ID;

          IRID rval = siblingSpecialWrite.getArg_RVal();

          if (pool[lval].tag != IRI_GEN::EnvBinding) {
            throw std::runtime_error(
                "Expected SiblingSpecialWrite to resolve to EnvBinding");
          }
          IRID writeStmt = LWriteSEXP::create(pool, lval, rval, true, false, false);

          pool.update_arg_inplace(bbID, stmtIDX, writeStmt);
          patchNode(file, pool, rval, bbScopeIDX);
        } else if (currStmtTag == IRI_GEN::EnvWrite) {
          EnvWriteSEXP ew(stmtID, pool);
          ResolveEnvBindingSEXP unresolvedBinding(ew.getArg_LValTarget(), pool);


          auto & resolved =  pool.iris->resolve(unresolvedBinding.getNAME(), bbScopeIDX);
          IRID lval = resolved.ID;
          IRID rval = ew.getArg_RVal();
          bool hasASW = resolved.isARGX;
          IRID target;
          if (pool[lval].tag == IRI_GEN::GlobalBinding || pool[lval].tag == IRI_GEN::ScriptBinding) {
            assert(ew.getTHISINIT() == false);
            target = GWriteSEXP::create(pool, lval, rval, ew.getSAFE(), false, false, false);
          } else if (pool[lval].tag == IRI_GEN::EnvBinding) {
            target = LWriteSEXP::create(pool, lval, rval, hasASW || ew.getSAFE(), false, ew.getTHISINIT());
          } else if (pool[lval].tag == IRI_GEN::RemoteEnvBinding) {
            RemoteEnvBindingSEXP rb(lval, pool);
            if (rb.hasMODULE() || rb.hasMODULEI() || rb.hasMODULENSI()) {
              assert(ew.getTHISINIT() == false);
              target = MWriteSEXP::create(pool, lval, rval, ew.getSAFE(), false);
            } else {
              target = RWriteSEXP::create(pool, lval, rval, ew.getSAFE(), false, ew.getTHISINIT());
            }
          }
          pool.update_arg_inplace(bbID, stmtIDX, target);
          patchNode(file, pool, rval, bbScopeIDX);
        } else {
          patchNode(file, pool, stmtID, bbScopeIDX);
        }
      }
      for (auto [stmtID, stmtIDX] : bb.stmts()) {
        IRI_TAG currStmtTag = pool[stmtID].tag;
        if (currStmtTag == LWrite) {
          LWriteSEXP lw(stmtID, pool);
          IRID ebID = lw.getArg_LValTarget();
          EnvBindingSEXP eb(ebID, pool);
          if ((*pool.iris)[eb.id].tombstone) {
            pool.update_arg_inplace(bbID, stmtIDX, pool.NOP_SEXP);
          }

        }

      }
    }
  }

  patchNode(file, pool, file.staticImports(), pool.iris->getTopLevelScope());
  patchNode(file, pool, file.staticExports(), pool.iris->getTopLevelScope());
  patchNode(file, pool, file.staticStarExports(), pool.iris->getTopLevelScope());

}
} // namespace IRI_CORE_PASSES
