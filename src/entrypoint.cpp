
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

IRISEXP runCorePasses(IRISEXP sexp, std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext)
{

  // Run Core Passes
  normalizeBBFlags(iridiumBuildContext);

  hoistFunctionDeclarations(sexp, iridiumBuildContext);

  filterNOPs(sexp);

  groupIntoClosureGroups(sexp, iridiumBuildContext);

  populateModuleBindings(sexp, iridiumBuildContext);

  populateImplicitBindings(sexp, iridiumBuildContext);

  reduceFunctionDeclarations(sexp, iridiumBuildContext);

  populateExplicitBindings(sexp, iridiumBuildContext);

  addClosureArgsBindings(sexp, iridiumBuildContext);

  initializeStackFrame(sexp, iridiumBuildContext);

  patchHeritageConstructorSuperCalls(sexp, iridiumBuildContext);

  reduceResolvePrivateEnvBindingSEXP(sexp, iridiumBuildContext);

  reduceResolveEnvBindingSEXP(sexp, iridiumBuildContext);

  resolveLambdaTargets(sexp, sexp, iridiumBuildContext, -1);

  resolveBreakAndContinueTargets(sexp, sexp, iridiumBuildContext);

  decorateReturnTargets(sexp, sexp, iridiumBuildContext);

  promoteAsyncReturns(sexp, iridiumBuildContext);

  markNamespaceImports(sexp, iridiumBuildContext);

  markSloppyWrites(sexp, iridiumBuildContext, -1);

  loosenWritestoASWs(sexp);

  markDirectEvals(sexp, iridiumBuildContext, -1);

  return sexp;
}

IRISEXP runOptPasses(std::shared_ptr<FileSEXP> fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext)
{
  // fileSEXP->prettyPrint(std::cout);
  // std::cout << std::endl;

  // FileView fileView(fileSEXP, iridiumBuildContext);
  // // // std::ostringstream test;
  // // // fileView.dumpSymbolTable(test);
  // // // std::cout << test.str() << std::endl;

  // // // fileSEXP->prettyPrint(std::cout);
  // // // std::cout << std::endl;

  // // // doRedundantGotoElimination(fileView);

  // // fileSEXP->prettyPrint(std::cout);
  // // std::cout << std::endl;

  // // doUnreadBindingRemoval(fileView, iridiumBuildContext);

  // return fileView.checkout();

  return fileSEXP;
}