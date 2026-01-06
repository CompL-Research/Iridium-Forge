
#include "Iridium/entrypoint.h"

#include <msgpack.hpp>

#include "Iridium/IridiumBuildContext.h"
#include "Iridium/IridiumSEXP.h"
#include "Iridium/Globals.h"
#include "Iridium/CorePasses/1_normailzeBBFlags.h"
#include "Iridium/CorePasses/2_hoistFunctionDeclarations.h"
#include "Iridium/CorePasses/3_filterNops.h"
#include "Iridium/CorePasses/4_1_groupIntoClosureGroups.h"
#include "Iridium/CorePasses/4_2_populateModuleBindings.h"
#include "Iridium/CorePasses/4_3_populateImplicitBindings.h"
#include "Iridium/CorePasses/4_4_reduceFunctionDeclarations.h"
#include "Iridium/CorePasses/4_5_populateExplicitBindings.h"
#include "Iridium/CorePasses/4_6_addClosureArgsBindings.h"
#include "Iridium/CorePasses/5_initializeStackFrame.h"
#include "Iridium/CorePasses/6_patchHeritageConstructorSuperCalls.h"
#include "Iridium/CorePasses/7_reduceResolvePrivateEnvBindingSEXP.h"
#include "Iridium/CorePasses/8_reduceResolveEnvBindingSEXP.h"
#include "Iridium/CorePasses/9_resolveLambdaTargets.h"
#include "Iridium/CorePasses/10_resolveBreakAndContinueTargets.h"
#include "Iridium/CorePasses/11_decorateReturnTargets.h"
#include "Iridium/CorePasses/12_promoteAsyncReturns.h"
#include "Iridium/CorePasses/13_markNamespaceImports.h"
#include "Iridium/CorePasses/14_markSloppyWrites.h"
#include "Iridium/CorePasses/15_loosenWritestoASWs.h"
#include "Iridium/CorePasses/16_markDirectEvals.h"
#include "Iridium/CorePasses/17_removeTDZChecksForSloppyGlobalWrites.h"
#include "Iridium/CorePasses/18_reduceDeleteNonGlobalBindingsToTrue.h"
#include "Iridium/CorePasses/19_canonicalBBs.h"
#include "Iridium/CorePasses/20_releaseEscapingBindingsFromScope.h"

#include "Iridium/Structure/FileView.h"

#include "Iridium/PassManager.h"

static std::set<double> taintedScopes;

IRISEXP runCorePasses(
    IRISEXP sexp,
    std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext)
{
  normalizeBBFlags(iridiumBuildContext);
  DBG("Completed normalizeBBFlags");

  hoistFunctionDeclarations(sexp, iridiumBuildContext);
  DBG("Completed hoistFunctionDeclarations");

  filterNOPs(sexp);
  DBG("Completed filterNOPs");

  groupIntoClosureGroups(sexp, iridiumBuildContext);
  DBG("Completed groupIntoClosureGroups");


  populateModuleBindings(sexp, iridiumBuildContext);
  DBG("Completed populateModuleBindings");

  populateImplicitBindings(sexp, iridiumBuildContext);
  DBG("Completed populateImplicitBindings");

  reduceFunctionDeclarations(sexp, iridiumBuildContext);
  DBG("Completed reduceFunctionDeclarations");

  populateExplicitBindings(sexp, iridiumBuildContext);
  DBG("Completed populateExplicitBindings");

  addClosureArgsBindings(sexp, iridiumBuildContext);
  DBG("Completed addClosureArgsBindings");

  initializeStackFrame(sexp, iridiumBuildContext);
  DBG("Completed initializeStackFrame");


  patchHeritageConstructorSuperCalls(sexp, iridiumBuildContext);
  DBG("Completed patchHeritageConstructorSuperCalls");


  reduceResolvePrivateEnvBindingSEXP(sexp, iridiumBuildContext);
  DBG("Completed reduceResolvePrivateEnvBindingSEXP");

  reduceResolveEnvBindingSEXP(sexp, iridiumBuildContext);
  DBG("Completed reduceResolveEnvBindingSEXP");

  resolveLambdaTargets(sexp, sexp, iridiumBuildContext, -1);
  DBG("Completed resolveLambdaTargets");

  resolveBreakAndContinueTargets(sexp, sexp, iridiumBuildContext);
  DBG("Completed resolveBreakAndContinueTargets");

  decorateReturnTargets(sexp, sexp, iridiumBuildContext);
  DBG("Completed decorateReturnTargets");

  promoteAsyncReturns(sexp, iridiumBuildContext);
  DBG("Completed promoteAsyncReturns");

  markNamespaceImports(sexp, iridiumBuildContext);
  DBG("Completed markNamespaceImports");

  markSloppyWrites(sexp, iridiumBuildContext, -1);
  DBG("Completed markSloppyWrites");

  loosenWritestoASWs(sexp);
  DBG("Completed loosenWritestoASWs");
  
  markDirectEvals(sexp, iridiumBuildContext, -1, taintedScopes);
  DBG("Completed markDirectEvals");

  removeTDZChecksForSloppyGlobalWrites(sexp, iridiumBuildContext);
  DBG("Completed removeTDZChecksForSloppyGlobalWrites");

  reduceDeleteNonGlobalBindingsToTrue(sexp, iridiumBuildContext);
  DBG("Completed reduceDeleteNonGlobalBindingsToTrue");

  canonicalBBs(sexp, iridiumBuildContext);
  DBG("Completed canonicalBBs");

  // Iterate over all declared bindings, if they are reachable from any tainted scope then add to the tainted list
  auto fileSEXP = std::dynamic_pointer_cast<FileSEXP>(sexp);
  assert(fileSEXP);

  for (auto & b : fileSEXP->args)
  {
    if (auto bbCont = std::dynamic_pointer_cast<BBContainerSEXP>(b))
    {
      auto bindings = std::dynamic_pointer_cast<BindingsSEXP>(bbCont->getBindings());
      assert(bindings);

      for (auto & binding : bindings->getLocalBindings()->args)
      {
        auto bbb = std::dynamic_pointer_cast<EnvBindingSEXP>(binding);
        assert(bbb);

        for (auto & ts : taintedScopes)
        {
          // std::cout << "tainted scope reachability check: TS" << ts << "->" << bbb->getScope() << ": " << isScopeReachable(ts, bbb->getScope(), iridiumBuildContext) << std::endl;
          if (isScopeReachable(ts, bbb->getScope(), iridiumBuildContext))
          {
            // Add all remote bindings to tainted scope
            auto bbContainer = getBBContainerSEXPByScopeId(fileSEXP, findParentClosureScope(ts, iridiumBuildContext));
            std::shared_ptr<BindingsSEXP> bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(bbContainer->getBindings());
            if (!bindingsSEXP)
              throw std::runtime_error("[adding remote bindings to evaled scope] Bindings not found for tainted scope");
            resolveScopedLookup(fileSEXP, iridiumBuildContext, bbb->getNAME(), ts, bindingsSEXP);
            FileView::dynamicEvaledBindings.insert(bbb);
            // std::cout << "Tainted Binding: " << bbb->getNAME() << std::endl;
          }
        }
      }
    }
  }
  DBG("Completed eval tainting");

  releaseEscapingBindingsFromScope(sexp, iridiumBuildContext);
  DBG("Completed releaseEscapingBindingsFromScope");

  // TODO: ensure no gotoSEXP has the "CONTINUE_TARGET" flag, this ensures we handled everything

  return sexp;
}

IRISEXP runOptPasses(std::shared_ptr<FileSEXP> fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext)
{
  if (getenv("NO_OPT")) return fileSEXP;
  FileView fileView(fileSEXP, iridiumBuildContext);
  PassManager pm(fileView, iridiumBuildContext);
  if (getenv("JUST_ANALYZE"))
  {
    std::stringstream ss;
    pm.justAnalysis(ss, taintedScopes);
  }
  else
  {
    pm.optimize(1, taintedScopes);
  }
  auto res = pm.checkout();
  filterNOPs(res);
  return res;

  // return fileSEXP;
}