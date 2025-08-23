#include "Iridium/Passes/1_normailzeBBFlags.h"
#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"
#include "Iridium/IridiumBuildContext.h"

void normalizeBBFlags(std::unordered_map<int, IRIBUILDCONTEXT> & buildContext) {
  for (auto & e : buildContext) {
    int scopeIdx = e.first;
    IRIBUILDCONTEXT buildContext = e.second;

    auto firstBB = std::dynamic_pointer_cast<BBSEXP>(buildContext->BB[0]);
    assert(firstBB && "Expected res to be a BBSEXP");

    BBSEXPFLAGS mainBBFlag = getBBFlag(firstBB);
    for (auto & bbHolder : buildContext->BB) {
      auto bb = std::dynamic_pointer_cast<BBSEXP>(bbHolder);
      assert(bb && "Expected res to be a BBSEXP");
      setBBFlag(bb, mainBBFlag);
    }
  }
}