#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Storage/StringPool.h"
#include "Support/BBSupport.hpp"
#include "Support/IRIS.hpp"
#include <cstdlib>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

enum class LOC { FRAME, MODULE, SCRIPT };

void _4_4_RFD(IridiumPool &pool, IRID fileID, BUILD_CTX &iridiumBuildContext) {
  StringID str_undefined = pool.strings.intern("undefined");

  FileSEXP fileSEXP(fileID, pool);

  bool isModule = iridiumBuildContext[0]->isModule;
  bool isScript = !isModule;
  double topLevelScope = pool.iris->getTopLevelScope();

  auto args = pool.get_args(fileID);

  for (auto &bbcID : args) {
    if (pool[bbcID].tag != IRI_GEN::BBContainer)
      continue;

    std::unordered_map<double, std::vector<IRID>> hoistedEnvWrites;

    BBContainerSEXP container(bbcID, pool);
    auto bbs = pool.get_args(container.getArg_BB());
    auto containerScope = container.getScopeIDX();

    bool isStrict = container.hasSTRICT();
    bool isSloppy = !isStrict;

    auto &containerBC = iridiumBuildContext[containerScope];

    for (auto &bbID : bbs) {
      BBSupport bb(bbID, pool);
      auto localScope = bb.getScopeIDX();

      bool isTopLevelClosureCTX = topLevelScope == containerScope;
      bool closureLevelDecl = localScope == containerScope;

      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        IRI_TAG currTag = pool[stmtID].tag;
        if (currTag == IRI_GEN::JSFuncDecl) {
          pool.update_arg_inplace(bbID, stmtOffset, pool.NOP_SEXP);

          JSFuncDeclSEXP jsfd(stmtID, pool);
          ResolveEnvBindingSEXP renvB(jsfd.getArg_LValTarget(), pool);
          IRI_STORAGE::StringID funName = renvB.getNAME();
          IRID rval = jsfd.getArg_RVal();

          auto sloppyFuncDecl = [&]() {
            IRID lval = pool.iris->declareScriptBinding(localScope, funName, IRI_GEN::JSVAR).ID;
            hoistedEnvWrites[localScope].push_back(GWriteSEXP::create(pool, lval, rval, false, false, false, true));
          };

          auto moduleFuncDecl = [&]() {
            IRID lval = pool.iris->declareRBinding(localScope, funName, IRI_GEN::JSLET, IRI_GEN::MODULE).ID;
            hoistedEnvWrites[localScope].push_back(MWriteSEXP::create(pool, lval, rval, true, false));
          };

          auto lexicalLetDecl = [&]() {
            IRID lval = pool.iris->declareLBinding(localScope, funName, IRI_GEN::JSLET).ID;
            hoistedEnvWrites[localScope].push_back(
              LWriteSEXP::create(pool, lval, rval, true, false, false)
            );
          };

          auto scriptVarAndGWrite = [&]() {
            IRID lval = pool.iris->declareScriptBinding(containerScope, funName, IRI_GEN::JSVAR).ID;
            hoistedEnvWrites[containerScope].push_back(
              GWriteSEXP::create(pool, lval, pool.NOP_SEXP, false, false, true, false)
            );
            hoistedEnvWrites[localScope].push_back(
              GWriteSEXP::create(pool, lval, rval, true, false, false, false)
            );
          };

          auto funcVarDecl = [&]() {
            IRID lval = pool.iris->declareLBinding(localScope, funName, IRI_GEN::JSVAR).ID;
            hoistedEnvWrites[localScope].push_back(
              LWriteSEXP::create(pool, lval, rval, true, false, false)
            );
          };

          auto funcVarAndGWrite = [&]() {
            IRID lval = pool.iris->declareLBinding(localScope, funName, IRI_GEN::JSVAR).ID;
            hoistedEnvWrites[containerScope].push_back(
              LWriteSEXP::create(pool, lval, IRI_HELPERS::createUnsafeEnvReadSEXP(pool, str_undefined), true, false, false)
            );
            hoistedEnvWrites[localScope].push_back(
              LWriteSEXP::create(pool, lval, rval, true, false, false)
            );
          };

          if (isTopLevelClosureCTX) {
            // File Level
            if (closureLevelDecl) {
              if (isScript) {
                sloppyFuncDecl();
              } else {
                moduleFuncDecl();
              }
            } else {
              if (isSloppy) {
                scriptVarAndGWrite();
              } else {
                lexicalLetDecl();
              }
            }
          } else {
            // Function Level
            if (closureLevelDecl) {
              funcVarDecl();
            } else {
              if (isSloppy) {
                funcVarAndGWrite();
              } else {
                lexicalLetDecl();
              }
            }
          }
        }
      }
    }
    // Handle Explicit Declarations
    for (auto &e : hoistedEnvWrites) {
      auto &containerBC = iridiumBuildContext[e.first];
      double localScope = containerBC->scopeIDX;
      double parentScope = containerBC->parent;
      IRID startBBID = containerBC->BB[0];
      pool.add_args_to_beginning(startBBID, e.second);
    }
  }
}
} // namespace IRI_CORE_PASSES
