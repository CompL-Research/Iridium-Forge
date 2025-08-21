// // 
// // Filter NOPs
// // 
// filterNOPs(currSEXP: IridiumSEXP) {
//   if (isBBSEXP(currSEXP)) {
//     currSEXP.args = currSEXP.args.filter(e => !isNOPSEXP(e));
//   } else {
//     currSEXP.args.forEach(e => this.filterNOPs(e));
//   }
// }

#include "Iridium/Passes/2_hoistFunctionDeclarations.h"
#include "Iridium/Globals.h"
#include "Iridium/IridiumTypes.h"

void filterNOPs(IRISEXP currSEXP)
{
  if (auto bb = std::dynamic_pointer_cast<BBSEXP>(currSEXP)) {
    auto & bbInsts = bb->args;
    bbInsts.erase(std::remove_if(bbInsts.begin(), bbInsts.end(),
                         [&](const auto &inst) { return std::dynamic_pointer_cast<NOPSEXP>(inst) != nullptr; }),
          bbInsts.end());
  } else {
    for (auto & e : currSEXP->args) filterNOPs(e);
  }
}