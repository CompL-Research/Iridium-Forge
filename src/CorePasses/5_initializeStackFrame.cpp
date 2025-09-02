#include "Iridium/CorePasses/5_initializeStackFrame.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void initializeStackFrame(IRISEXP fileSexp, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  for (auto &bbc : fileSexp->args)
  {
    auto container = std::dynamic_pointer_cast<BBContainerSEXP>(bbc);
    if (!container)
      continue;
    balanceStackFrame(container, iridiumBuildContext);
  }
}