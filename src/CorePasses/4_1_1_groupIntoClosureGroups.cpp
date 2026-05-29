
#include "Config.h"
#include "CorePasses.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Parser/IridiumBuildContext.h"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline void setClosureFlags(double flag, BBContainerSEXP bbContainer) {
  bbContainer.setContainerFlagID(flag);

  // ::func_kind::
  // GENERATOR && ASYNC = JS_FUNC_ASYNC_GENERATOR
  // GENERATOR          = JS_FUNC_GENERATOR
  // ASYNC              = JS_FUNC_ASYNC

  // ::has_simple_parameter_list::

  // has_prototype:                PROTO
  // is_derived_class_constructor: DERIVED
  // need_home_object:             HOME
  // new_target_allowed:           NEW
  // super_call_allowed:           SCALL
  // super_allowed:                SOBJ
  // arguments_allowed:            ARGUMENTS

  switch ((int)flag) {
  case CF_TOP_LEVEL_MODULE:
    bbContainer.setARGUMENTS();
    break;

  case CF_TOP_LEVEL_SCRIPT:
    bbContainer.setARGUMENTS();
    break;

  case CF_ARROW_FUNCTION:
    bbContainer.setARGUMENTS();
    break;

  case CF_FUNCTION:
    bbContainer.setPROTO();
    bbContainer.setNEW();
    bbContainer.setARGUMENTS();
    break;

  case CF_CTR:
    bbContainer.setHOME();
    bbContainer.setNEW();
    bbContainer.setSOBJ();
    bbContainer.setARGUMENTS();
    break;

  case CF_DERIVED_CTR:
    bbContainer.setDERIVED();
    bbContainer.setHOME();
    bbContainer.setNEW();
    bbContainer.setSCALL();
    bbContainer.setSOBJ();
    bbContainer.setARGUMENTS();
    break;

  case CF_CLASS_METHOD:
    bbContainer.setHOME();
    bbContainer.setNEW();
    bbContainer.setSOBJ();
    bbContainer.setARGUMENTS();
    break;

  case CF_PROP_INIT:
    bbContainer.setHOME();
    bbContainer.setNEW();
    bbContainer.setSOBJ();
    break;

  default:
    throw std::runtime_error("Invalid closure flag");
  }
}

inline IRID createBBContainerSEXP(IridiumPool &pool, double targetScopeIDX,
                                  BUILD_CTX &iridiumBuildContext) {
  auto parent =
      IRI_HELPERS::getLexicalScope(pool, targetScopeIDX, iridiumBuildContext);
  bool isTopLevel = parent == -1;

  IRID localBindingsID =
      ListSEXP::create(pool, pool.strings.intern("EnvBinding"));
  IRID remoteBindingsID =
      ListSEXP::create(pool, pool.strings.intern("RemoteEnvBinding"));
  IRID poolBindingsID =
      ListSEXP::create(pool, pool.strings.intern("PoolBinding"));

  IRID bindingsID = BindingsSEXP::create(pool, localBindingsID, remoteBindingsID, poolBindingsID);

  IRID BBListID = ListSEXP::create(pool, pool.strings.intern("BB"));

  auto &currContext = iridiumBuildContext[targetScopeIDX];

  IRID bbContainer = BBContainerSEXP::create(
      pool, bindingsID, BBListID, pool.strings.intern(currContext->name), false,
      currContext->isAsync, currContext->isStrict, currContext->isGenerator,
      false, false, false, false, false, false, isTopLevel,
      currContext->ecmaArgs, BBSEXP(currContext->BB[0], pool).getIDX(),
      targetScopeIDX, -1);

  setClosureFlags(currContext->kind, BBContainerSEXP(bbContainer, pool));
  return bbContainer;
}

void _4_1_1_GICG(IridiumPool &pool, IRID fileSEXP,
                 BUILD_CTX &iridiumBuildContext) {

  std::unordered_map<double, IRID> bbGroups;
  std::unordered_map<IRID, std::vector<IRID>> hhGroupArgs;

  auto bbs = pool.get_args(fileSEXP);

  for (auto &bbID : bbs) {
    BBSEXP currBB(bbID, pool);

    auto targetScopeIDX = IRI_HELPERS::findParentClosureScope(
        pool, currBB.getScopeIDX(), iridiumBuildContext);

    if (bbGroups.find(targetScopeIDX) == bbGroups.end()) {
      bbGroups[targetScopeIDX] =
          createBBContainerSEXP(pool, targetScopeIDX, iridiumBuildContext);
    }

    hhGroupArgs[BBContainerSEXP(bbGroups[targetScopeIDX], pool).getArg_BB()]
        .push_back(bbID);
  }

  std::vector<IRID> bbContainers;
  for (auto &e : bbGroups)
    bbContainers.push_back(e.second);
  pool.set_args(fileSEXP, bbContainers);

  for (auto &e : hhGroupArgs) {
    pool.set_args(e.first, e.second);
  }

  // Other files referenced by this module
  std::vector<IRID> moduleRequestsVec;
  auto moduleRequests =
      ListSEXP::create(pool, pool.strings.intern("ModuleRequest"));

  // Objects imported by this module
  std::vector<IRID> staticImportsVec;
  auto staticImports =
      ListSEXP::create(pool, pool.strings.intern("StaticImport"));

  // Objects exported by this module
  std::vector<IRID> staticExportsVec;
  auto staticExports = ListSEXP::create(pool, pool.strings.intern(""));

  // Reexports by this module
  std::vector<IRID> staticStarExportsVec;
  auto staticStarExports =
      ListSEXP::create(pool, pool.strings.intern("StarExport"));

  pool.set_args(moduleRequests, moduleRequestsVec);
  pool.set_args(staticImports, staticImportsVec);
  pool.set_args(staticExports, staticExportsVec);
  pool.set_args(staticStarExports, staticStarExportsVec);
  pool.add_args_to_beginning(fileSEXP, {moduleRequests, staticImports,
                                        staticExports, staticStarExports});

}
} // namespace IRI_CORE_PASSES
