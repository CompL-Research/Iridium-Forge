#pragma once
#include "Iridium/Globals.h"

void reduceResolvePrivateEnvBindingSEXP(IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, double currBBScope = 0);