#include "Iridium/PassManager.h"

#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/DataflowSolver.h"
#include "Iridium/OptimizationPasses/UnreadBindingRemoval.h"
#include "Iridium/OptimizationPasses/ConstantProp.h"
#include "Iridium/OptimizationPasses/WriteBarrierReduction.h"

void PassManager::optimize(int level)
{
  for (auto & bbContView : fileView.bbContainerViews)
  {
    
    // std::cout << "==Entry Block==" << std::endl;
    // bbContView.cfgManager.cfg[bbContView.cfgManager.entryBlock()]->prettyPrint(std::cout);
    // std::cout << "===============" << std::endl;
    DataflowSolver<ConstantsAtStmt> constantsAtStmtSolver(bbContView.cfgManager.cfg, bbContView.cfgManager.entryBlock(), true, [&]() {
      return ConstantsAtStmt::bottom();
    });
    // auto res = constantsAtStmtSolver.run(ConstantsAtStmt::boundary());

    for (auto & e : constantsAtStmtSolver.run(ConstantsAtStmt::boundary()))
    {
      ConstantProp::Transform(bbContView.cfgManager.cfg[e.first], e.second);
    }

    auto uncapturedStackBindings = bbContView.getUncapturedStackBindings();
    DataflowSolver<TDZA> tdzaSolver(bbContView.cfgManager.cfg, bbContView.cfgManager.entryBlock(), true, [&]() {
      return TDZA::bottom(uncapturedStackBindings);
    });
    // auto res = tdzaSolver.run(TDZA::boundary(bindingsUnderAnalysis));

    for (auto & e : tdzaSolver.run(TDZA::boundary(uncapturedStackBindings)))
    {
      WriteBarrierReduction::Transform(bbContView.cfgManager.cfg[e.first], e.second);
    }

    // std::set<std::shared_ptr<EnvBindingSEXP>> capturedStackBindings = bbContView.getCapturedStackBindings();
    // DataflowSolver<Liveness> livenessSolver(bbContView.cfgManager.cfg, bbContView.cfgManager.entryBlock(), false, [&]() {
    //   return Liveness::bottom();
    // });
    // // auto res = tdzaSolver.run(TDZA::boundary(bindingsUnderAnalysis));

    // for (auto & e : livenessSolver.run(Liveness::boundary(capturedStackBindings)))
    // {
    //   // WriteBarrierReduction::Transform(bbContView.cfgManager.cfg[e.first], e.second);
    // }

    
    // std::ostringstream ss;
    // for (auto & e : res)
    // {
    //   e.second.transferDump(bbContView.cfgManager.cfg[e.first], ss);
    // }
    // std::cout << ss.str() << std::endl;

    // for (auto & e : res)
    // {
    //   std::ostringstream ss;
    //   ss << std::endl << "OUT(BB): " << bbContView.cfgManager.cfg[e.first]->getIDX() << std::endl;
    //   e.second.dump(ss);
    //   ss << std::endl;
    //   std::cout << ss.str();
    // }

  }

  fileView.refreshSymbolTable();
  
  doUnreadBindingRemoval(fileView, iridiumBuildContext);

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