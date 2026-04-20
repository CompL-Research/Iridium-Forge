

#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/BindingsSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline void patchNode(IridiumPool &pool, IRID node, double scopeToTaint,
                      double containerScope) {
  auto args = pool.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    patchNode(pool, args[i], scopeToTaint, containerScope);
  }
}

void _17_RTDZ(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);

    auto containerScope = bbc.getScopeIDX();

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);

      auto bbScope = bb.getScopeIDX();

      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        auto stmtTag = pool[stmtID].tag;
        if (stmtTag == IRI_GEN::StackReject) {
          IRID tdzReadID = pool.get_args_view(stmtID)[0];
          if (pool[tdzReadID].tag == IRI_GEN::TDZRead) {
            TDZReadSEXP tdzRead(tdzReadID, pool);

            // Strict mode, let the check be there, reduce to a normal read
            if (bbc.hasSTRICT()) {
              pool.update_arg_inplace(bbID, stmtOffset,
                                      EnvReadSEXP::create(pool,
                                                          tdzRead.getArg_Obj(),
                                                          tdzRead.hasSAFE()));
              continue;
            }

            IRID obj = tdzRead.getArg_Obj();
            if (pool[obj].tag == IRI_GEN::GlobalBinding) {
              GlobalBindingSEXP gg(obj, pool);
              if (!gg.hasSLOPPYDECL())
                pool.update_arg_inplace(bbID, stmtOffset, pool.NOP_SEXP);
              else
                pool.update_arg_inplace(bbID, stmtOffset,
                                        EnvReadSEXP::create(pool,
                                                            tdzRead.getArg_Obj(),
                                                            tdzRead.hasSAFE()));
            } else {
              pool.update_arg_inplace(bbID, stmtOffset,
                                      EnvReadSEXP::create(pool,
                                                          tdzRead.getArg_Obj(),
                                                          tdzRead.hasSAFE()));
            }
          }
        }
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
