#pragma once
#include <memory>
#include <vector>
#include <string>
#include <variant>
#include <functional>


// Forward decls of your types
class IridiumSEXP;
class IridiumFlag;
class BBSEXP;
class EnvBindingSEXP;
class RemoteEnvBindingSEXP;


typedef std::shared_ptr<IridiumSEXP> IRISEXP;

std::shared_ptr<IridiumFlag> cloneFlag(const std::shared_ptr<IridiumFlag>& f);
using SpecialCloneHandler = std::function<IRISEXP(const IRISEXP&)>;
IRISEXP cloneIRISEXP(const IRISEXP& node, const SpecialCloneHandler& handler);
std::shared_ptr<BBSEXP> cloneBB(
  std::shared_ptr<BBSEXP> bb,
  std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::shared_ptr<EnvBindingSEXP>> & localIndirectionMap, 
  std::unordered_map<std::shared_ptr<RemoteEnvBindingSEXP>, std::shared_ptr<RemoteEnvBindingSEXP>> & remoteIndirectionMap
);