#pragma once
#include "Iridium/Globals.h"

void patchHeritageConstructorSuperCalls(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);