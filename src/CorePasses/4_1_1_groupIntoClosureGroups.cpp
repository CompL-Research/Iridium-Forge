
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

inline IRID createBBContainerSEXP(IRIContext &ctx, double targetScopeIDX,
                                  BUILD_CTX &iridiumBuildContext) {
  auto parent =
      IRI_HELPERS::getLexicalScope(ctx, targetScopeIDX, iridiumBuildContext);
  bool isTopLevel = parent == -1;

  IRID localBindingsID =
      ListSEXP::create(ctx, ctx.storage.strings.intern("EnvBinding"));
  IRID remoteBindingsID =
      ListSEXP::create(ctx, ctx.storage.strings.intern("RemoteEnvBinding"));
  IRID poolBindingsID =
      ListSEXP::create(ctx, ctx.storage.strings.intern("PoolBinding"));

  IRID bindingsID = BindingsSEXP::create(ctx, localBindingsID, remoteBindingsID, poolBindingsID);

  IRID BBListID = ListSEXP::create(ctx, ctx.storage.strings.intern("BB"));

  auto &currContext = iridiumBuildContext[targetScopeIDX];

  IRID bbContainer = BBContainerSEXP::create(
      ctx, bindingsID, BBListID, ctx.storage.strings.intern(currContext->name), false,
      currContext->isAsync, currContext->isStrict, currContext->isGenerator,
      false, false, false, false, false, false, isTopLevel,
      currContext->ecmaArgs, BBSEXP(currContext->BB[0], ctx).getIDX(),
      targetScopeIDX, -1);

  setClosureFlags(currContext->kind, BBContainerSEXP(bbContainer, ctx));
  return bbContainer;
}

void _4_1_1_GICG(IRIContext &ctx, IRID fileSEXP,
                 BUILD_CTX &iridiumBuildContext) {

  std::unordered_map<double, IRID> bbGroups;
  std::unordered_map<IRID, std::vector<IRID>> hhGroupArgs;

  auto bbs = ctx.storage.nodes.get_args(fileSEXP);

  for (auto &bbID : bbs) {
    BBSEXP currBB(bbID, ctx);

    auto targetScopeIDX = IRI_HELPERS::findParentClosureScope(
        ctx, currBB.getScopeIDX(), iridiumBuildContext);

    if (bbGroups.find(targetScopeIDX) == bbGroups.end()) {
      bbGroups[targetScopeIDX] =
          createBBContainerSEXP(ctx, targetScopeIDX, iridiumBuildContext);
    }

    hhGroupArgs[BBContainerSEXP(bbGroups[targetScopeIDX], ctx).getArg_BB()]
        .push_back(bbID);
  }

  std::vector<IRID> bbContainers;
  for (auto &e : bbGroups)
    bbContainers.push_back(e.second);
  ctx.storage.nodes.set_args(fileSEXP, bbContainers);

  for (auto &e : hhGroupArgs) {
    ctx.storage.nodes.set_args(e.first, e.second);
  }

  // Other files referenced by this module
  std::vector<IRID> moduleRequestsVec;
  auto moduleRequests =
      ListSEXP::create(ctx, ctx.storage.strings.intern("ModuleRequest"));

  // Objects imported by this module
  std::vector<IRID> staticImportsVec;
  auto staticImports =
      ListSEXP::create(ctx, ctx.storage.strings.intern("StaticImport"));

  // Objects exported by this module
  std::vector<IRID> staticExportsVec;
  auto staticExports = ListSEXP::create(ctx, ctx.storage.strings.intern(""));

  // Reexports by this module
  std::vector<IRID> staticStarExportsVec;
  auto staticStarExports =
      ListSEXP::create(ctx, ctx.storage.strings.intern("StarExport"));

  ctx.storage.nodes.set_args(moduleRequests, moduleRequestsVec);
  ctx.storage.nodes.set_args(staticImports, staticImportsVec);
  ctx.storage.nodes.set_args(staticExports, staticExportsVec);
  ctx.storage.nodes.set_args(staticStarExports, staticStarExportsVec);
  ctx.storage.nodes.add_args_to_beginning(fileSEXP, {moduleRequests, staticImports,
                                        staticExports, staticStarExports});

}
} // namespace IRI_CORE_PASSES
