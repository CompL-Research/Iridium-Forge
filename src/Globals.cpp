#include "Iridium/Globals.h"
#include "Iridium/IridiumSEXP.h"
#include "Iridium/IridiumBuildContext.h"

std::unordered_map<int, std::shared_ptr<IridiumSEXP>> bbIdxToSEXPMap;
std::unordered_map<int, std::shared_ptr<IridiumBuildContext>> iridiumBuildContext;