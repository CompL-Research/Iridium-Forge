#include "Iridium/CorePasses/17_removeTDZChecksForSloppyGlobalWrites.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

void removeTDZChecksForSloppyGlobalWrites(IRISEXP file, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{
  auto fileSEXP = std::dynamic_pointer_cast<FileSEXP>(file);
  assert(fileSEXP && "Expected FileSEXP");
  for (auto bbCont : fileSEXP->args)
  {
    auto bbContainerSEXP = std::dynamic_pointer_cast<BBContainerSEXP>(bbCont);
    if (!bbContainerSEXP)
      continue;

    for (auto bb : bbContainerSEXP->getBB()->args)
    {
      auto bbSEXP = std::dynamic_pointer_cast<BBSEXP>(bb);
      assert(bbSEXP && "Expected BBSEXP");
      for (int i = 0; i < bb->args.size(); i++) {
        if (auto sRej = std::dynamic_pointer_cast<StackRejectSEXP>(bb->args.at(i))) {
          if (auto tdzRead = std::dynamic_pointer_cast<TDZReadSEXP>(sRej->args.at(0))) {

            // Strict mode, let the check be there, reduce to a normal read
            if (bbContainerSEXP->hasSTRICT()) {
              tdzRead->tag = "EnvRead";
              sRej->args.at(0) = EnvReadSEXP::generateFrom(tdzRead);
              continue;
            }

            // Sloppy mode, remove check for global binding write, reduce to normal read otherwise
            if (std::dynamic_pointer_cast<GlobalBindingSEXP>(tdzRead->getObj())) {
              bb->args.at(i) = std::make_shared<NOPSEXP>();
            } else {
              tdzRead->tag = "EnvRead";
              sRej->args.at(0) = EnvReadSEXP::generateFrom(tdzRead);
            }
          }
        }
      }
    }
  }
}