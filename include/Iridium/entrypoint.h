#pragma once
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"
#include <msgpack.hpp>
#include <string>

IRISEXP runCorePasses(IRISEXP sexp, std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext);
IRISEXP runOptPasses(std::shared_ptr<FileSEXP> fileSEXP, std::unordered_map<int, IRIBUILDCONTEXT> iridiumBuildContext);