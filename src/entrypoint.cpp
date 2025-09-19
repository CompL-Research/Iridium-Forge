
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

#include "Iridium/Structure/FileView.h"
#include "Iridium/OptimizationPasses/UnreadBindingRemoval.h"
#include "Iridium/PassManager.h"

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

  markDirectEvals(sexp, iridiumBuildContext, -1);
  DBG("Completed markDirectEvals");

  return sexp;
}

IRISEXP runOptPasses(std::shared_ptr<FileSEXP> fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext)
{
  FileView fileView(fileSEXP, iridiumBuildContext);
  PassManager pm(fileView, iridiumBuildContext);
  pm.optimize(1);
  auto res = pm.checkout();
  
  filterNOPs(res);
  
  return res;
}