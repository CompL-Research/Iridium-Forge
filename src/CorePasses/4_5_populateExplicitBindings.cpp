#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Storage/StringPool.h"
#include <cstdlib>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline bool sameKindFlag(EnvBindingSEXP binding, IRI_FLAG kindFlag) {
  if (kindFlag == IRI_FLAG::JSLET && binding.hasJSLET())
    return true;
  if (kindFlag == IRI_FLAG::JSCONST && binding.hasJSCONST())
    return true;
  if (kindFlag == IRI_FLAG::JSVAR && binding.hasJSVAR())
    return true;
  if (kindFlag == IRI_FLAG::JSARG && binding.hasJSARG())
    return true;
  if (kindFlag == IRI_FLAG::JSRESTARG && binding.hasJSRESTARG())
    return true;
  return false;
}

inline bool hasBindingReference(IridiumPool &pool, BindingsSEXP bindingsSEXP,
                                double idx, StringID name,
                                IRI_GEN::IRI_FLAG kindFlag, double localScope,
                                double parentScope) {
  auto localBindingsArgs = pool.get_args(bindingsSEXP.getArg_LocalBindings());
  auto remoteBindingsArgs = pool.get_args(bindingsSEXP.getArg_RemoteBindings());
  for (auto &b : localBindingsArgs) {
    EnvBindingSEXP bin(b, pool);
    if (bin.getIDX() == idx && bin.getNAME() == name &&
        sameKindFlag(bin, kindFlag) && bin.getScope() == localScope &&
        bin.getParentScope() == parentScope)
      return true;
  }

  for (auto &b : remoteBindingsArgs) {
    RemoteEnvBindingSEXP rbin(b, pool);
    EnvBindingSEXP bin(IRI_HELPERS::resolveRemoteBinding(pool, b), pool);
    if (bin.getIDX() == idx && bin.getNAME() == name &&
        sameKindFlag(bin, kindFlag) && bin.getScope() == localScope &&
        bin.getParentScope() == parentScope)
      return true;
  }

  return false;
}

void _4_5_PEB(IridiumPool &pool, IRID fileID, BUILD_CTX &iridiumBuildContext) {
  bool USE_TOP_LEVEL_LOCALS = getenv("FORCE_TOP_LEVEL_LOCALS") ? true : false;
  FileSEXP fileSEXP(fileID, pool);

  std::unordered_map<IRID, std::vector<IRID>> prefixVector;
  std::unordered_map<IRID, std::vector<IRID>> postfixVector;

  bool isModule = iridiumBuildContext[0]->isModule;
  double topLevelScope = IRI_HELPERS::getTopLevelScope(pool, fileID);

  auto args = pool.get_args(fileID);

  for (auto &bbcID : args) {
    if (pool[bbcID].tag != IRI_GEN::BBContainer)
      continue;

    std::vector<IRID> sloppyDeclarations;
    std::unordered_map<double,
                       std::vector<std::tuple<IRI_STORAGE::StringID,
                                              IRI_GEN::IRI_FLAG, bool, bool>>>
        explicitBindings;

    BBContainerSEXP container(bbcID, pool);
    auto bbs = pool.get_args(container.getArg_BB());
    auto containerScope = container.getScopeIDX();

    auto &containerBC = iridiumBuildContext[containerScope];

    BindingsSEXP bindingsSEXP(container.getArg_Bindings(), pool);

    IRI_STORAGE::StringID str_arguments     = pool.strings.intern("arguments");
    IRI_STORAGE::StringID str_closureName   = pool.strings.intern(containerBC->name);
    bool declaresArguments    = false;
    IRID envBindingArguments;
    bool declaresClosureName  = false;
    IRID envBindingClosureName;

    std::set<StringID> argsRedecl;
    for (auto &a : containerBC->args)
      argsRedecl.insert(pool.strings.intern(a));

    auto lbList = pool.get_args(bindingsSEXP.getArg_LocalBindings());
    for (auto &eb : lbList) {
      EnvBindingSEXP ebs(eb, pool);
      if (ebs.getNAME() == str_arguments) {
        declaresArguments = true;
        envBindingArguments = eb;
      } else if (ebs.getNAME() == str_closureName) {
        declaresClosureName = true;
        envBindingClosureName = eb;
      }
    }

    for (auto &bbID : bbs) {
      BBSEXP bb(bbID, pool);
      auto stmts = pool.get_args(bbID);

      auto localScope = bb.getScopeIDX();
      auto directParent = iridiumBuildContext[localScope]->parent;
      auto varHoistingScope = IRI_HELPERS::findVARHoistingScope(
          pool, localScope, iridiumBuildContext);

      for (size_t i = 0; i < stmts.size(); i++) {
        IRID stmtID = stmts[i];
        IRI_TAG currTag = pool[stmtID].tag;

        bool doInit = true;
        if (currTag == IRI_GEN::JSExplicitBindingDeclarationN) {
          doInit = false;
          pool.update_tag(stmtID, JSExplicitBindingDeclaration);
          currTag = JSExplicitBindingDeclaration;
        }

        if (currTag == IRI_GEN::JSExplicitBindingDeclaration) {
          JSExplicitBindingDeclarationSEXP jsExpBD(stmtID, pool);
          double scopeToHoistTo;
          IRI_FLAG flag;
          if (jsExpBD.hasJSLET()) {
            scopeToHoistTo = localScope;
            flag = IRI_GEN::JSLET;
          } else if (jsExpBD.hasJSCONST()) {
            scopeToHoistTo = localScope;
            flag = IRI_GEN::JSCONST;
          } else if (jsExpBD.hasJSVAR()) {
            scopeToHoistTo = varHoistingScope;
            flag = IRI_GEN::JSVAR;
          } else
            throw std::runtime_error("[Forge]: Unknown explicit binding kind!");

          bool possibleArgRedl = scopeToHoistTo == varHoistingScope && directParent == containerScope;

          ResolveEnvBindingSEXP binding(jsExpBD.getArg_LValTarget(), pool);

          if (possibleArgRedl && argsRedecl.contains(binding.getNAME())) {
            if (jsExpBD.hasArg_RVal()) {
              auto envWrite = EnvWriteSEXP::create(
                pool, jsExpBD.getArg_LValTarget(), jsExpBD.getArg_RVal(),
                jsExpBD.hasSLOPPY(), jsExpBD.getSAFE(),
                jsExpBD.getTHISINIT());

              pool.update_arg_inplace(bbID, i, envWrite);
            } else {
              pool.update_arg_inplace(bbID, i, NOPSEXP::create(pool));
            }
            continue;
          }

          if (possibleArgRedl && declaresArguments && binding.getNAME() == str_arguments) {
            if (flag == JSVAR) {
            //   EnvBindingSEXP ebs(envBindingArguments, pool);
            //   ebs.setNAME(pool.strings.intern("<arguments-shadowed>"));
            // } else {
              if (jsExpBD.hasArg_RVal()) {
                auto envWrite = EnvWriteSEXP::create(
                  pool, jsExpBD.getArg_LValTarget(), jsExpBD.getArg_RVal(),
                  jsExpBD.hasSLOPPY(), jsExpBD.getSAFE(),
                  jsExpBD.getTHISINIT());

                pool.update_arg_inplace(bbID, i, envWrite);
              } else {
                pool.update_arg_inplace(bbID, i, NOPSEXP::create(pool));
              }
              continue;
            }
          }

          if (possibleArgRedl && declaresClosureName && binding.getNAME() == str_closureName) {
            EnvBindingSEXP ebs(envBindingClosureName, pool);
            ebs.setNAME(pool.strings.intern("<closureName-shadowed>"));
          }

          if (!isModule && scopeToHoistTo == topLevelScope) {
            if (USE_TOP_LEVEL_LOCALS) {
              explicitBindings[scopeToHoistTo].push_back(std::make_tuple(
                  binding.getNAME(), flag, doInit, binding.hasASW()));
            } else {
              IRID globalBindingID =
                  pool.getGlobalBindingSEXP(binding.getNAME());
              GlobalBindingSEXP gBinding(globalBindingID, pool);
              gBinding.setSLOPPYDECL();
              sloppyDeclarations.push_back(JSSloppyDeclSEXP::create(
                  pool, binding.getNAME(), flag == JSLET, flag == JSCONST,
                  flag == JSVAR));
            }
          } else {
            explicitBindings[scopeToHoistTo].push_back(std::make_tuple(
                binding.getNAME(), flag, doInit, binding.hasASW()));
          }

          if (!jsExpBD.hasArg_RVal()) {
            if (jsExpBD.hasJSVAR() || doInit == false) {
              pool.update_arg_inplace(bbID, i, pool.NOP_SEXP);
            } else {
              auto envWrite = EnvWriteSEXP::create(
                  pool, jsExpBD.getArg_LValTarget(), pool.UNDEF_READ,
                  jsExpBD.hasSLOPPY(), jsExpBD.getSAFE(),
                  jsExpBD.getTHISINIT());

              pool.update_arg_inplace(bbID, i, envWrite);
            }
          } else {
            auto envWrite = EnvWriteSEXP::create(
                pool, jsExpBD.getArg_LValTarget(), jsExpBD.getArg_RVal(),
                jsExpBD.hasSLOPPY(), jsExpBD.getSAFE(),
                jsExpBD.getTHISINIT());

            pool.update_arg_inplace(bbID, i, envWrite);
          }
        }
      }
    }
    {
      // Handle Sloppy Declarations
      IRID startBB = containerBC->BB[0];
      pool.add_args_to_beginning(startBB, sloppyDeclarations);
    }
    {

      std::vector<IRID> remoteBindings;
      std::vector<IRID> localBindings;
      // Handle Explicit Declarations
      for (auto &e : explicitBindings) {
        auto &bindingContext = iridiumBuildContext[e.first];
        double localScope = bindingContext->scopeIDX;
        double parentScope = bindingContext->parent;

        IRID startBBID = bindingContext->BB[0];
        BBSEXP startBB(startBBID, pool);
        std::vector<IRID> hoistedEnvWrites;

        for (auto &b : e.second) {
          auto &[bindingName, kindFlag, doInit, isASW] = b;
          IRID lval = ResolveEnvBindingSEXP::create(pool, bindingName, false);
          IRID rval;
          if (kindFlag == JSVAR)
            rval = pool.UNDEF_READ;
          else
            rval = pool.NUBD_SEXP;

          if (!hasBindingReference(pool, bindingsSEXP, containerScope,
                                   bindingName, kindFlag, localScope,
                                   parentScope)) {
            if (startBB.hasTopLevel() && USE_TOP_LEVEL_LOCALS == false) {
              IRID localBinding = EnvBindingSEXP::create(
                  pool, bindingName, isASW, false, false, kindFlag == JSLET,
                  kindFlag == JSCONST, kindFlag == JSVAR, false,
                  containerBC->scopeIDX, -1, localScope, parentScope, -1);
              IRID remoteBinding = RemoteEnvBindingSEXP::create(
                  pool, localBinding, false, false, true, -1);
              remoteBindings.push_back(remoteBinding);
              if (doInit) {
                hoistedEnvWrites.push_back(EnvWriteSEXP::create(
                    pool, lval, rval, false, true, false));
              }
            } else {
              IRID localBinding = EnvBindingSEXP::create(
                  pool, bindingName, isASW, false, false, kindFlag == JSLET,
                  kindFlag == JSCONST, kindFlag == JSVAR, false,
                  containerBC->scopeIDX, -1, localScope, parentScope, -1);
              localBindings.push_back(localBinding);
              if (doInit) {
                hoistedEnvWrites.push_back(EnvWriteSEXP::create(
                    pool, lval, rval, false, true, false));
              }
            }
          }
        }
        pool.add_args_to_beginning(startBBID, hoistedEnvWrites);
      }
      pool.add_args_to_beginning(bindingsSEXP.getArg_LocalBindings(),
                                 localBindings);
      pool.add_args_to_beginning(bindingsSEXP.getArg_RemoteBindings(),
                                 remoteBindings);
    }
  }
}
} // namespace IRI_CORE_PASSES
