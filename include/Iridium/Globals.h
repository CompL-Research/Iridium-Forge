#pragma once
#include <unordered_map>
#include <memory>

class IridiumSEXP;
class IridiumBuildContext;
typedef std::shared_ptr<IridiumSEXP> IRISEXP;
extern std::unordered_map<int, std::shared_ptr<IridiumSEXP>> bbIdxToSEXPMap;
extern std::unordered_map<int, std::shared_ptr<IridiumBuildContext>> iridiumBuildContext;