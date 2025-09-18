#include "Iridium/CorePasses/6_patchHeritageConstructorSuperCalls.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"


std::vector<IRISEXP> heritageThisInit(std::string thisValHolder, std::string propInitClos)
{
  std::vector<IRISEXP> res;

  // IRISEXP LValTarget, IRISEXP RVal, bool SLOPPY, bool SAFE, bool THISINIT
  res.push_back(
      std::make_shared<EnvWriteSEXP>(
          std::make_shared<ResolveEnvBindingSEXP>("this", false),
          std::make_shared<EnvReadSEXP>(std::make_shared<ResolveEnvBindingSEXP>(thisValHolder, false), false),
          false,
          false,
          true // <- This is about the only place where we set THISINIT flag to true
          ));

  // bool CCall, bool ConstructorCall, bool PrivateCall, bool Import, bool Super, bool V8Intrinsic, double JSDirectEval
  auto callSEXP = std::make_shared<CallSiteSEXP>(true, false, false, false, false, false, 0);

  callSEXP->args.push_back(
      std::make_shared<EnvReadSEXP>(std::make_shared<ResolveEnvBindingSEXP>("this", false), false));

  callSEXP->args.push_back(
      std::make_shared<EnvReadSEXP>(std::make_shared<ResolveEnvBindingSEXP>(propInitClos, false), false));

  auto stackReject = std::make_shared<StackRejectSEXP>(1);
  stackReject->args.push_back(
      callSEXP);

  res.push_back(stackReject);
  return res;
}

bool predicateSuperCallCheck(const IRISEXP &ele)
{
  if (auto callEle = std::dynamic_pointer_cast<CallSiteSEXP>(ele))
  {
    return callEle->hasSuper();
  }
  return false;
}

void patchHeritageConstructorSuperCalls(IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  if (std::dynamic_pointer_cast<BindingsSEXP>(currSEXP))
    return;

  if (auto bb = std::dynamic_pointer_cast<BBSEXP>(currSEXP))
  {
    std::set<IRISEXP> superCalls;
    if (iridiumBuildContext.find(bb->getScopeIDX()) == iridiumBuildContext.end())
      throw std::runtime_error("patching super failed, invalid getScopeIDX");

    auto &closureScope = iridiumBuildContext[bb->getScopeIDX()];

    // Collect stmts that contain a super() constructor call
    for (auto &stmt : currSEXP->args)
    {
      if (hasNode(stmt, predicateSuperCallCheck))
      {
        superCalls.insert(stmt);
      }
    }

    // Process each super call
    for (auto &scallHolder : superCalls)
    {
      // Walk up context chain
      IRIBUILDCONTEXT &buildContext = closureScope;

      while (true)
      {
        if (buildContext->kind == getDerivedConstructorClosureFlag())
        {
          // super should be wrapped in EnvWriteSEXP
          auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(scallHolder);
          if (!envWrite)
            throw std::runtime_error("Expected super value to be stored inside an EnvWriteSEXP");

          auto lValHolder = envWrite->getLValTarget();
          auto resolveEnv = std::dynamic_pointer_cast<ResolveEnvBindingSEXP>(lValHolder);
          if (!resolveEnv)
            throw std::runtime_error("Expected LVals to be unresolved while super calls are patched");

          if (!buildContext->propInitClos)
            throw std::runtime_error("Constructors with heritage are expected to have propInitClos");

          // Insert heritage init after the super() call
          auto heritageInitSeq = heritageThisInit(resolveEnv->getNAME(), buildContext->propInitClos.value());

          insertAfter(currSEXP->args, scallHolder, heritageInitSeq);

          break;
        }
        else
        {
          if (iridiumBuildContext.find(buildContext->parent) != iridiumBuildContext.end())
          {
            buildContext = iridiumBuildContext[buildContext->parent];
          }
          else
          {
            throw std::runtime_error("buildContext is undefined, failed to patch super");
          }
        }
      }
    }
  }
  else
  {
    for (auto & e : currSEXP->args) patchHeritageConstructorSuperCalls(e, iridiumBuildContext);
  }
}