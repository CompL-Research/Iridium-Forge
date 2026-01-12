#include "Iridium/CorePasses/21_markTLA.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void markTLA(IRISEXP file, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  auto hasAwait = [&](const IRISEXP &node) -> bool {
    return std::dynamic_pointer_cast<AwaitSEXP>(node) ? true : false;
  };

  auto fileSEXP = std::dynamic_pointer_cast<FileSEXP>(file);
  assert(fileSEXP && "Expected FileSEXP");
  for (auto bbCont : fileSEXP->args)
  {
    auto bbContainerSEXP = std::dynamic_pointer_cast<BBContainerSEXP>(bbCont);
    if (!bbContainerSEXP)
      continue;
    
    if (!bbContainerSEXP->hasTopLevel())
      continue;

    for (auto bb : bbContainerSEXP->getBB()->args)
    {
      auto bbSEXP = std::dynamic_pointer_cast<BBSEXP>(bb);
      assert(bbSEXP && "Expected BBSEXP");
      std::vector<IRISEXP> & currArgs = bbSEXP->args;
      for (int i = 0; i < currArgs.size(); i++) {
        if (hasNode(currArgs.at(i), hasAwait)) {
          fileSEXP->setTLA();
          return;
        }
      }
    }
  }
}