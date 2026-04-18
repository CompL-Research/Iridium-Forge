
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
  switch ((int)flag) {
  case 0:
    throw std::runtime_error("Invalid closure flag");
  case 1:
    break;
  case 2:
    bbContainer.setPROTO();
    bbContainer.setNEW();
    break; // flags.push("PROTO", "NEW");
  case 3:
    bbContainer.setPROTO();
    bbContainer.setNEW();
    bbContainer.setSCALL();
    bbContainer.setSOBJ();
    bbContainer.setHOME();
    bbContainer.setDERIVED();
    break; // flags.push("PROTO", "NEW", "SCALL", "SOBJ", "HOME", "DERIVED");
  case 4:
    bbContainer.setSOBJ();
    bbContainer.setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 5:
    bbContainer.setHOME();
    break; // flags.push("HOME");
  case 6:
    bbContainer.setNEW();
    break;
  case 7:
    bbContainer.setNEW();
    bbContainer.setSOBJ();
    bbContainer.setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 8:
    bbContainer.setNEW();
    bbContainer.setHOME();
    break; // flags.push("HOME");
  case 9:
    bbContainer.setNEW();
    bbContainer.setSOBJ();
    bbContainer.setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 10:
    bbContainer.setSOBJ();
    bbContainer.setHOME();
    break; // flags.push("SOBJ", "HOME")
  case 11:
    bbContainer.setNEW();
    bbContainer.setSOBJ();
    bbContainer.setHOME();
    break;
  case 12:
    bbContainer.setSOBJ();
    bbContainer.setHOME();
    break; // flags.push("SOBJ", "HOME");
  case 13:
    bbContainer.setARGUMENTS();
    break;
  default:
    throw std::runtime_error("expected a valid closure flag");
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

  IRID bindingsID = BindingsSEXP::create(
      pool, localBindingsID, remoteBindingsID, poolBindingsID, parent);

  IRID BBListID = ListSEXP::create(pool, pool.strings.intern("BB"));

  auto &currContext = iridiumBuildContext[targetScopeIDX];

  IRID bbContainer = BBContainerSEXP::create(
      pool, bindingsID, BBListID, pool.strings.intern(currContext->name),
      false, currContext->isAsync, currContext->isStrict,
      currContext->isGenerator, false, false, false, false, false, false,
      isTopLevel, currContext->ecmaArgs,
      BBSEXP(currContext->BB[0], pool).getIDX(), targetScopeIDX, -1);

  setClosureFlags(currContext->kind, BBContainerSEXP(bbContainer, pool));
  return bbContainer;
}

void _4_1_GICG(IridiumPool &pool, IRID fileSEXP, BUILD_CTX &iridiumBuildContext) {

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
}
} // namespace IRI_CORE_PASSES
