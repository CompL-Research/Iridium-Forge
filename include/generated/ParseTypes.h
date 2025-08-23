#pragma once
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

class ParseIridiumTypes {
public:
  IRISEXP specialize(IRISEXP obj) {
    auto & tag = obj->tag;
    if (tag == "File")
    {
      return FileSEXP::generateFrom(obj);
    }

    throw std::runtime_error("ParseIridiumTypes::specialize unhandled Tag: " + tag);
  }
};