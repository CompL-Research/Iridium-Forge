#include "Iridium/Cloning.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

std::shared_ptr<IridiumFlag> cloneFlag(const std::shared_ptr<IridiumFlag> &f)
{
  if (!f)
    return nullptr;
  return std::make_shared<IridiumFlag>(*f); // relies on default copy ctor
}

// Special handler type: you can inject custom behavior for certain tags
using SpecialCloneHandler = std::function<IRISEXP(const IRISEXP &)>;

IRISEXP cloneIRISEXP(const IRISEXP &node, const SpecialCloneHandler &handler)
{
  if (!node)
    return nullptr;

  // Intercept special tags
  if ((node->tag == "EnvBinding" || node->tag == "RemoteEnvBinding") && handler)
  {
    return handler(node);
  }

  // Naive clone
  auto newNode = std::make_shared<IridiumSEXP>();
  newNode->tag = node->tag;

  // Clone args
  for (auto &arg : node->args)
  {
    newNode->args.push_back(cloneIRISEXP(arg, handler));
  }

  // Clone flags
  for (auto &fl : node->flags)
  {
    newNode->flags.push_back(cloneFlag(fl));
  }

  return newNode;
}

std::shared_ptr<BBSEXP> cloneBB(
    std::shared_ptr<BBSEXP> bb,
    std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::shared_ptr<EnvBindingSEXP>> &localIndirectionMap,
    std::unordered_map<std::shared_ptr<RemoteEnvBindingSEXP>, std::shared_ptr<RemoteEnvBindingSEXP>> &remoteIndirectionMap)
{
  // bool TopLevel, bool ClosureBoundary, bool Lexical, double IDX, double ScopeIDX
  std::shared_ptr<BBSEXP> res = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), bb->getIDX(), bb->getScopeIDX());

  SpecialCloneHandler handler = [&](const IRISEXP &node)
  {
    if (auto envBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(node))
    {
      assert(localIndirectionMap.count(envBinding) > 0);
      return std::static_pointer_cast<IridiumSEXP>(localIndirectionMap[envBinding]);
    }
    else if (auto remoteEnvBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(node))
    {
      assert(remoteIndirectionMap.count(remoteEnvBinding) > 0);
      return std::static_pointer_cast<IridiumSEXP>(remoteIndirectionMap[remoteEnvBinding]);
    }

    throw std::runtime_error("Unexpected patch case while cloning BB");
  };

  for (auto &stmt : bb->args)
  {
    res->args.push_back(
        cloneIRISEXP(
            stmt,
            handler));
  }

  return res;
}