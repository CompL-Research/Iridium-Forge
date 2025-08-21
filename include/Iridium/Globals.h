#pragma once
#include <unordered_map>
#include <memory>

class IridiumSEXP;
class IridiumBuildContext;
typedef std::shared_ptr<IridiumSEXP> IRISEXP;
typedef std::shared_ptr<IridiumBuildContext> IRIBUILDCONTEXT;
extern std::unordered_map<int, IRISEXP> bbIdxToSEXPMap;
extern std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext;