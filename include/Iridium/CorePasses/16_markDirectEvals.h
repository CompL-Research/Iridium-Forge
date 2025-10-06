#pragma once
#include "Iridium/Globals.h"

void markDirectEvals(IRISEXP currSEXP, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext, double currBBScope, std::set<double> & taintedScopes);