#include "Iridium/CorePasses/19_canonicalBBs.h"
#include "Iridium/Globals.h"
#include "generated/IridiumTypes.h"

#define PRINT_REMOVED_STMT_INFO 1

// 
// This pass:
// 1. There is only one control flow statement in a BB
// 2. Each BB ends with a control flow statement
//    - ReturnAsync
//    - Goto
//    - IfElseJump
//    - Ret
//    - Return
//    - Throw
// 

bool isCFlowStmt(IRISEXP stmt) {
  if (std::dynamic_pointer_cast<ReturnAsyncSEXP>(stmt)) return true;
  if (std::dynamic_pointer_cast<GotoSEXP>(stmt)) return true;
  if (std::dynamic_pointer_cast<IfElseJumpSEXP>(stmt)) return true;
  if (std::dynamic_pointer_cast<RetSEXP>(stmt)) return true;
  if (std::dynamic_pointer_cast<ReturnSEXP>(stmt)) return true;
  if (std::dynamic_pointer_cast<ThrowSEXP>(stmt)) return true;
  return false;
}

void canonicalBBs(IRISEXP file, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
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
      std::vector<IRISEXP> & currArgs = bbSEXP->args;
      for (int i = 0; i < currArgs.size(); i++) {
        IRISEXP curr = currArgs.at(i);
        if (isCFlowStmt(curr)) {

          #if PRINT_REMOVED_STMT_INFO == 1
          if (i + 1 > currArgs.size()) {
            std::cerr << "Removing Statments from BB(" << bbSEXP->getIDX() << ")" << std::endl;
            for (int j = (i + 1); j < currArgs.size(); j++) currArgs.at(j)->prettyPrint(std::cout, 2);
          }
          #endif
          currArgs.erase(currArgs.begin() + i + 1, currArgs.end());
          break;
        }
      }
      assert(isCFlowStmt(currArgs.back()) && "Last STMT expected to a CFLOW Stmt");
    }
  }
}