#include "Iridium/PassManager.h"

#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/Domains/CopyPropInfo.h"
#include "Iridium/Analysis/DataflowSolver.h"
#include "Iridium/OptimizationPasses/ConstantProp.h"
#include "Iridium/OptimizationPasses/WriteBarrierReduction.h"
#include "Iridium/OptimizationPasses/CopyProp.h"
#include "Iridium/OptimizationPasses/DCE.h"
#include "Iridium/OptimizationPasses/DeadBindingRemoval.h"
#include "Iridium/CorePasses/3_filterNops.h"

void PassManager::optimize(int level)
{
  for (int i = 0; i < 5; i++)
  {
    DBG("Iter: " + std::to_string(i));
    for (auto &bbContView : fileView.bbContainerViews)
    {
      fileView.refreshSymbolTable();
      auto capturedStackBindings = bbContView.getCapturedStackBindings();
      auto uncapturedStackBindings = bbContView.getUncapturedStackBindings();

      CopyPropInfo::blacklist = capturedStackBindings;
      Liveness::blacklist = capturedStackBindings;

      {
        DataflowSolver<ConstantsAtStmt> constantsAtStmtSolver(bbContView.cfgManager, true, [&]()
                                                              { return ConstantsAtStmt::bottom(); });
        for (auto &e : constantsAtStmtSolver.run(ConstantsAtStmt::boundary()))
        {
          ConstantProp::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
      }

      {
        DataflowSolver<CopyPropInfo> copyPropInfoSolver(bbContView.cfgManager, true, [&]()
                                                        { return CopyPropInfo(); });
        for (auto &e : copyPropInfoSolver.run(CopyPropInfo()))
        {
          auto currBB = bbContView.cfgManager.cfg[e.first];
          CopyProp::Transform(currBB, e.second);
        }
      }

      {
        DataflowSolver<TDZA> tdzaSolver(bbContView.cfgManager, true, [&]()
                                        { return TDZA::bottom(uncapturedStackBindings); });
        for (auto &e : tdzaSolver.run(TDZA::boundary(uncapturedStackBindings)))
        {
          WriteBarrierReduction::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
      }

      {
        DataflowSolver<Liveness> livenessSolver(bbContView.cfgManager, false, [&]()
                                                { return Liveness::bottom(); });
        for (auto &e : livenessSolver.run(Liveness::boundary(capturedStackBindings)))
        {
          auto currBB = bbContView.cfgManager.cfg[e.first];
          DCE::Transform(currBB, e.second);
          filterNOPs(currBB);
        }
      }

      doDeadBindingRemoval(fileView);
    }
  }
}

std::shared_ptr<FileSEXP> PassManager::checkout()
{
  return fileView.checkout();
}