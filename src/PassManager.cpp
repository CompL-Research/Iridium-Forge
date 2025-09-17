#include "Iridium/PassManager.h"
// #include "Iridium/OptimizationPasses/UnreadBindingRemoval.h"

#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Analysis/DataflowSolver.h"

void PassManager::optimize(int level)
{
  for (auto & bbContView : fileView.bbContainerViews)
  {
    
    // std::cout << "==Entry Block==" << std::endl;
    // bbContView.cfgManager.cfg[bbContView.cfgManager.entryBlock()]->prettyPrint(std::cout);
    // std::cout << "===============" << std::endl;
    // DataflowSolver<ConstantsAtStmt> constantsAtStmtSolver(bbContView.cfgManager.cfg, bbContView.cfgManager.entryBlock(), true, [&]() {
    //   return ConstantsAtStmt::bottom();
    // });
    // auto res = constantsAtStmtSolver.run(ConstantsAtStmt::boundary());
    
    // for (auto & e : res)
    // {
    //   std::ostringstream ss;
    //   ss << std::endl << "OUT(BB): " << bbContView.cfgManager.cfg[e.first]->getIDX() << std::endl;
    //   e.second.dump(ss);
    //   ss << std::endl;
    //   std::cout << ss.str();
    // }

    auto bindingsUnderAnalysis = bbContView.getUncapturedStackBindings();

    DataflowSolver<TDZA> tdzaSolver(bbContView.cfgManager.cfg, bbContView.cfgManager.entryBlock(), true, [&]() {
      return TDZA::bottom(bindingsUnderAnalysis);
    });
    auto res = tdzaSolver.run(TDZA::boundary(bindingsUnderAnalysis));
    
    // std::ostringstream ss;
    // for (auto & e : res)
    // {
    //   e.second.transferDump(bbContView.cfgManager.cfg[e.first], ss);
    // }
    // std::cout << ss.str() << std::endl;

    for (auto & e : res)
    {
      std::ostringstream ss;
      ss << std::endl << "OUT(BB): " << bbContView.cfgManager.cfg[e.first]->getIDX() << std::endl;
      e.second.dump(ss);
      ss << std::endl;
      std::cout << ss.str();
    }

  }
  
  // doUnreadBindingRemoval(fileView, iridiumBuildContext);

  // switch (level)
  // {
  // case 0:
  //   break;
  // case 1:
  // // TODO
  //   passes.push_back(doUnreadBindingRemoval);  
  //   DBG("Completed UnreadBindingRemoval");
    
  // default:
  //   break;
  // }

  

}

std::shared_ptr<FileSEXP> PassManager::checkout()
{
  return fileView.checkout();
}