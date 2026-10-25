#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
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

void _4_5_PEB(IRIContext &ctx, IRID fileID, BUILD_CTX &iridiumBuildContext) {
  StringID str_undefined = ctx.storage.strings.intern("undefined");
  StringID str_arguments = ctx.storage.strings.intern("arguments");
  FileSEXP fileSEXP(fileID, ctx);
  bool isModule = iridiumBuildContext[0]->isModule;
  double topLevelScope = ctx.iris->getTopLevelScope();

  auto args = ctx.storage.nodes.get_args(fileID);

  bool IRI_TEMPS_ALWAYS_ON_STACK = true;

  for (auto &bbcID : args) {
    if (IRI_NODE(ctx, bbcID).tag != IRI_GEN::BBContainer)
      continue;

    std::unordered_map<double, std::vector<IRID>> scopewiseHoisting;

    BBContainerSEXP container(bbcID, ctx);
    auto bbs = ctx.storage.nodes.get_args(container.getArg_BB());
    auto containerScope = container.getScopeIDX();

    auto &containerBC = iridiumBuildContext[containerScope];

    for (auto &bbID : bbs) {
      BBSupport bb(bbID, ctx);

      auto localScope = bb.getScopeIDX();
      auto directParent = iridiumBuildContext[localScope]->parent;
      auto varHoistingScope = IRI_HELPERS::findVARHoistingScope(
          ctx, localScope, iridiumBuildContext);

      for (auto [stmtID, stmtOffset] : bb.stmts()) {
        IRI_TAG currTag = IRI_NODE(ctx, stmtID).tag;
        bool doHoist = true;
        bool isArgX = false;

        if (currTag == IRI_GEN::JSExplicitBindingDeclarationN) {
          doHoist = false;
          ctx.storage.nodes.update_tag(stmtID, JSExplicitBindingDeclaration);
          currTag = JSExplicitBindingDeclaration;
        }

        if (currTag == IRI_GEN::JSExplicitBindingDeclarationX) {
          isArgX = true;
          doHoist = false;
          ctx.storage.nodes.update_tag(stmtID, JSExplicitBindingDeclaration);
          currTag = JSExplicitBindingDeclaration;
        }

        if (currTag == IRI_GEN::JSExplicitBindingDeclaration) {
          JSExplicitBindingDeclarationSEXP jsExpBD(stmtID, ctx);
          double scopeToHoistTo;
          IRI_FLAG kind;
          if (jsExpBD.hasJSLET()) {
            scopeToHoistTo = localScope;
            kind = JSLET;
          } else if (jsExpBD.hasJSCONST()) {
            scopeToHoistTo = localScope;
            kind = JSCONST;
          } else if (jsExpBD.hasJSVAR()) {
            scopeToHoistTo = varHoistingScope;
            kind = JSVAR;
          } else
            throw std::runtime_error("[Forge]: Unknown explicit binding kind!");

          LOC loc;
          ResolveEnvBindingSEXP binding(jsExpBD.getArg_LValTarget(), ctx);
          StringID currBindingName = binding.getNAME();

          if (scopeToHoistTo == topLevelScope) {
            if (IRI_TEMPS_ALWAYS_ON_STACK && ctx.storage.strings.get(currBindingName).starts_with("~$") && !ctx.iris->isExportedBinding(currBindingName)) {
              loc = LOC::FRAME;
            } else if (isModule) {
              loc = LOC::MODULE;
            } else {
              loc = LOC::SCRIPT;
            }
          } else {
            loc = LOC::FRAME;
          }

          IRID resolvedLVAL;

          {
            //
            // Add binding to IRIS and add hoist declarations
            //
            IRI_STORAGE::StringID bindingName = binding.getNAME();
            IRID target, rval;
            if (kind == JSVAR)
              rval = IRI_HELPERS::createUnsafeEnvReadSEXP(ctx, str_undefined);
            else
              rval = ctx.storage.nodes.NUBD_SEXP;

            // Prevent duplicate declaration of var bindings and functions
            bool alreadyHasBinding = ctx.iris->hasBinding(bindingName, scopeToHoistTo);

            if (alreadyHasBinding && bindingName == str_arguments) {
              auto & decl = ctx.iris->resolve(bindingName, scopeToHoistTo);
              resolvedLVAL = decl.ID;
            } else {
              if (loc == LOC::FRAME) {
                auto & decl = ctx.iris->declareLBinding(scopeToHoistTo, bindingName, kind);
                if (isArgX) decl.isARGX = true;
                resolvedLVAL = decl.ID;
                target = LWriteSEXP::create(ctx, resolvedLVAL, rval, true, false, false);
              } else if (loc == LOC::MODULE) {
                auto & decl = ctx.iris->declareRBinding(scopeToHoistTo, bindingName, kind, IRI_GEN::MODULE);
                if (isArgX) decl.isARGX = true;
                resolvedLVAL = decl.ID;
                target = MWriteSEXP::create(ctx, resolvedLVAL, rval, true, false);
              } else if (loc == LOC::SCRIPT) {
                auto & decl = ctx.iris->declareScriptBinding(scopeToHoistTo, bindingName, kind);
                if (isArgX) decl.isARGX = true;
                resolvedLVAL = decl.ID;
                target = GWriteSEXP::create(ctx, resolvedLVAL, rval, false, false, true, false);
                doHoist = true;
              }
              if (alreadyHasBinding == false && doHoist == true) {
                scopewiseHoisting[scopeToHoistTo].push_back(target);
              }
            }
          }

          IRID inlineReplacement, irRVal;
          if (jsExpBD.hasArg_RVal()) {
            irRVal = jsExpBD.getArg_RVal();
          } else {
            irRVal = IRI_HELPERS::createUnsafeEnvReadSEXP(ctx, str_undefined);
          }

          if (!jsExpBD.hasArg_RVal() && (jsExpBD.hasJSVAR() || doHoist == false)) {
            inlineReplacement = ctx.storage.nodes.NOP_SEXP;
          } else {
            if (IRI_NODE(ctx, resolvedLVAL).tag == IRI_GEN::EnvBinding) {
              inlineReplacement = LWriteSEXP::create(ctx, resolvedLVAL, irRVal, true, false, false);
            } else if (IRI_NODE(ctx, resolvedLVAL).tag == IRI_GEN::RemoteEnvBinding) {
              inlineReplacement = MWriteSEXP::create(ctx, resolvedLVAL, irRVal, true, false);
            } else if (IRI_NODE(ctx, resolvedLVAL).tag == IRI_GEN::ScriptBinding) {
              inlineReplacement = GWriteSEXP::create(ctx, resolvedLVAL, irRVal, true, false, false, false);
            } else {
              throw std::runtime_error("Unexpected binding kind");
            }
          }
          ctx.storage.nodes.update_arg_inplace(bbID, stmtOffset, inlineReplacement);
        }
      }
    }
    // Handle Explicit Declarations
    for (auto &e : scopewiseHoisting) {
      auto &containerBC = iridiumBuildContext[e.first];
      double localScope = containerBC->scopeIDX;
      double parentScope = containerBC->parent;
      IRID startBBID = containerBC->BB[0];
      ctx.storage.nodes.add_args_to_beginning(startBBID, e.second);
    }
  }
}
} // namespace IRI_CORE_PASSES
