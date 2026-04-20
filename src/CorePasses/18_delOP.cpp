

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

inline void patchNode(IridiumPool &pool, IRID node, double startScope) {
  auto args = pool.get_args(node);

  for (int i = 0; i < args.size(); i++) {
    auto currID = args[i];
    if (pool[currID].tag == JSUnop) {
      JSUnopSEXP unop(currID, pool);
      IRID val = unop.getArg_Val();
      if (pool[val].tag == IRI_GEN::UNOPDelVar) {
        UNOPDelVarSEXP dv(val, pool);
        if (!pool.iris->isGlobal(dv.getNAME(), startScope))
          pool.update_arg_inplace(node, i, pool.FALSE_SEXP);
      }
    }
    patchNode(pool, args[i], startScope);
  }
}

void _18_DELOP(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
              BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);
    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);
      auto bbScope = bb.getScopeIDX();
      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        patchNode(pool, stmtID, bbScope);
      }
    }
  }
}
} // namespace IRI_CORE_PASSES
