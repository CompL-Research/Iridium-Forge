#pragma once
#include "Iridium/Globals.h"

void resolveBreakAndContinueTargets(IRISEXP fileSEXP, IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);