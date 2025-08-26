
#include "Iridium/entrypoint.h"

#include <msgpack.hpp>

#include "Iridium/IridiumBuildContext.h"
#include "Iridium/IridiumSEXP.h"
#include "Iridium/Globals.h"
#include "Iridium/Passes/1_normailzeBBFlags.h"
#include "Iridium/Passes/2_hoistFunctionDeclarations.h"
#include "Iridium/Passes/3_filterNops.h"
#include "Iridium/Passes/4_1_groupIntoClosureGroups.h"
#include "Iridium/Passes/4_2_populateModuleBindings.h"
#include "Iridium/Passes/4_3_populateImplicitBindings.h"
#include "Iridium/Passes/4_4_reduceFunctionDeclarations.h"
#include "Iridium/Passes/4_5_populateExplicitBindings.h"
#include "Iridium/Passes/4_6_addClosureArgsBindings.h"
#include "Iridium/Passes/5_initializeStackFrame.h"
#include "Iridium/Passes/6_patchHeritageConstructorSuperCalls.h"
#include "Iridium/Passes/7_reduceResolvePrivateEnvBindingSEXP.h"
#include "Iridium/Passes/8_reduceResolveEnvBindingSEXP.h"
#include "Iridium/Passes/9_resolveLambdaTargets.h"
#include "Iridium/Passes/10_resolveBreakAndContinueTargets.h"
#include "Iridium/Passes/11_decorateReturnTargets.h"
#include "Iridium/Passes/12_promoteAsyncReturns.h"
#include "Iridium/Passes/13_markNamespaceImports.h"
#include "Iridium/Passes/14_markSloppyWrites.h"
#include "Iridium/Passes/15_loosenWritestoASWs.h"
#include "Iridium/Passes/16_markDirectEvals.h"

IRISEXP runCorePasses(msgpack::object & iridiumObj, msgpack::object & buildContexts)
{

  // Read Iridium SEXP
  std::unordered_map<int, std::shared_ptr<BBSEXP>> bbIdxToSEXPMap;
  std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;
  auto sexp = parseSEXP(iridiumObj, &bbIdxToSEXPMap);

  // Read Iridium Build Contexts
  parseBuildContexts(buildContexts, bbIdxToSEXPMap, iridiumBuildContext);

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

