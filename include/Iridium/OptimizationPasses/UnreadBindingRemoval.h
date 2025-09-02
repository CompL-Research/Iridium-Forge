#pragma once
#include "Iridium/Structure/FileView.h"

void doUnreadBindingRemoval(FileView &fv, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext);