#pragma once
#include "Iridium/Globals.h"
#include <msgpack.hpp>
#include <string>

IRISEXP runCorePasses(msgpack::object & iridiumObj, msgpack::object & buildContexts);