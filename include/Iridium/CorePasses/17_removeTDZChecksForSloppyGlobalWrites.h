#pragma once
#include "Iridium/Globals.h"

void removeTDZChecksForSloppyGlobalWrites(IRISEXP file, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);