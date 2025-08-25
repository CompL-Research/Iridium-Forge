#pragma once
#include "Iridium/Globals.h"

void resolveLambdaTargets(IRISEXP fileSEXP, IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, double currBBscope = 0);