
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

// Toggle debug output here
// #define IRIDIUM_DEBUG 1  

#ifdef IRIDIUM_DEBUG
#define DBG(msg) \
    do { std::cerr << "[DEBUG] " << msg << std::endl; } while (0)
#else
#define DBG(msg) \
    do { } while (0)
#endif

IRISEXP runCorePasses(
    IRISEXP sexp,
    std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext)
{
    DBG("Starting runCorePasses");

    normalizeBBFlags(iridiumBuildContext);
    DBG("After normalizeBBFlags");

    hoistFunctionDeclarations(sexp, iridiumBuildContext);
    DBG("After hoistFunctionDeclarations");

    filterNOPs(sexp);
    DBG("After filterNOPs");

    groupIntoClosureGroups(sexp, iridiumBuildContext);
    DBG("After groupIntoClosureGroups");

    populateModuleBindings(sexp, iridiumBuildContext);
    DBG("After populateModuleBindings");

    populateImplicitBindings(sexp, iridiumBuildContext);
    DBG("After populateImplicitBindings");

    reduceFunctionDeclarations(sexp, iridiumBuildContext);
    DBG("After reduceFunctionDeclarations");

    populateExplicitBindings(sexp, iridiumBuildContext);
    DBG("After populateExplicitBindings");

    addClosureArgsBindings(sexp, iridiumBuildContext);
    DBG("After addClosureArgsBindings");

    initializeStackFrame(sexp, iridiumBuildContext);
    DBG("After initializeStackFrame");

    patchHeritageConstructorSuperCalls(sexp, iridiumBuildContext);
    DBG("After patchHeritageConstructorSuperCalls");

    reduceResolvePrivateEnvBindingSEXP(sexp, iridiumBuildContext);
    DBG("After reduceResolvePrivateEnvBindingSEXP");

    reduceResolveEnvBindingSEXP(sexp, iridiumBuildContext);
    DBG("After reduceResolveEnvBindingSEXP");

    resolveLambdaTargets(sexp, sexp, iridiumBuildContext, -1);
    DBG("After resolveLambdaTargets");

    resolveBreakAndContinueTargets(sexp, sexp, iridiumBuildContext);
    DBG("After resolveBreakAndContinueTargets");

    decorateReturnTargets(sexp, sexp, iridiumBuildContext);
    DBG("After decorateReturnTargets");

    promoteAsyncReturns(sexp, iridiumBuildContext);
    DBG("After promoteAsyncReturns");

    markNamespaceImports(sexp, iridiumBuildContext);
    DBG("After markNamespaceImports");

    markSloppyWrites(sexp, iridiumBuildContext, -1);
    DBG("After markSloppyWrites");

    loosenWritestoASWs(sexp);
    DBG("After loosenWritestoASWs");

    markDirectEvals(sexp, iridiumBuildContext, -1);
    DBG("After markDirectEvals");

    DBG("Finished runCorePasses");
    return sexp;
}

IRISEXP runOptPasses(std::shared_ptr<FileSEXP> fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext)
{
  // fileSEXP->prettyPrint(std::cout);
  // std::cout << std::endl;

  FileView fileView(fileSEXP, iridiumBuildContext);
  doUnreadBindingRemoval(fileView, iridiumBuildContext);
  auto res = fileView.checkout();
  return res;
  
  // return fileSEXP;
}