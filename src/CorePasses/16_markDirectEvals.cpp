

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
#include <iostream>
#include <stdexcept>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using namespace IRI_STRUCTURAL;

using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

static inline bool isGenericCall(CallSiteSEXP &o) {
  if (o.hasImport())
    return false;
  else if (o.hasSuper())
    return false;
  else if (o.hasV8Intrinsic())
    return false;
  else if (o.hasCCall())
    return false;
  else if (o.hasConstructorCall())
    return false;
  else if (o.hasPrivateCall())
    return false;
  else if (o.hasJSDirectEval())
    return false;
  else
    return true;
}

static inline bool isGenericCall(ApplySEXP &o) {
  if (o.hasConstructorCall())
    return false;
  return true;
}

inline void patchNode(IridiumPool &pool, IRID node, double scopeToTaint,
                      double containerScope) {
  auto args = pool.get_args(node);

  IRI_STORAGE::StringID EVAL = pool.strings.intern("eval");

  if (pool[node].tag == IRI_GEN::CallSite) {
    CallSiteSEXP callSite(node, pool);
    if (isGenericCall(callSite)) {
      if (pool.get_args(node).size() == 0) {
        throw std::runtime_error("Call site with no callee!!");
      }
      auto calleeID = pool.get_args(node)[0];
      if (pool[calleeID].tag == IRI_GEN::EnvRead) {
        EnvReadSEXP eRead(calleeID, pool);
        auto calleeBindingID = eRead.getArg_Obj();
        if (pool[calleeBindingID].tag == IRI_GEN::GlobalBinding) {
          GlobalBindingSEXP gBinding(calleeBindingID, pool);
          if (gBinding.getNAME() == EVAL) {
            pool.iris->taintScope(scopeToTaint);
            double evalREFIDX =
                pool.iris->getJSEvalLookupREFIDX(scopeToTaint, containerScope);
            callSite.setJSDirectEval(evalREFIDX);
          }
        }
      }
    }
  }

  if (pool[node].tag == IRI_GEN::Apply) {
    ApplySEXP callSite(node, pool);
    if (isGenericCall(callSite)) {
      if (pool.get_args(node).size() == 0) {
        throw std::runtime_error("Apply Call site with no callee!!");
      }
      auto calleeID = pool.get_args(node)[0];
      if (pool[calleeID].tag == IRI_GEN::EnvRead) {
        EnvReadSEXP eRead(calleeID, pool);
        auto calleeBindingID = eRead.getArg_Obj();
        if (pool[calleeBindingID].tag == IRI_GEN::GlobalBinding) {
          GlobalBindingSEXP gBinding(calleeBindingID, pool);
          if (gBinding.getNAME() == EVAL) {
            pool.iris->taintScope(scopeToTaint);

            double evalREFIDX =
                pool.iris->getJSEvalLookupREFIDX(scopeToTaint, containerScope);
            callSite.setJSDirectEval(evalREFIDX);
          }
        }
      }
    }
  }

  for (int i = 0; i < args.size(); i++) {
    patchNode(pool, args[i], scopeToTaint, containerScope);
  }
}

void _16_MDE(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP,
             BUILD_CTX &iridiumBuildContext) {

  FileSupport file(fileSEXP, pool);
  for (auto [bbcID, _] : file.containers()) {
    BBContainerSupport bbc(bbcID, pool);

    auto containerScope = bbc.getScopeIDX();

    for (auto [bbID, _] : bbc.bbs()) {
      BBSupport bb(bbID, pool);

      auto bbScope = bb.getScopeIDX();

      for (auto [stmtID, _] : bb.stmts()) {
        auto stmtTag = pool[stmtID].tag;
        patchNode(pool, stmtID, bbScope, containerScope);
      }
    }
  }

#ifdef PRINT_TAINT_TREE
  std::cout << "Scope tree after eval tainting (†)" << std::endl;
  pool.iris->dumpFlat(std::cout);
#endif
}
} // namespace IRI_CORE_PASSES
