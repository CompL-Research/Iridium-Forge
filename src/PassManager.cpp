#include "Iridium/PassManager.h"

#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/Domains/CopyPropInfo.h"
#include "Iridium/Analysis/DataflowSolver.h"
#include "Iridium/OptimizationPasses/UnreadBindingRemoval.h"
#include "Iridium/OptimizationPasses/ConstantProp.h"
#include "Iridium/OptimizationPasses/WriteBarrierReduction.h"
#include "Iridium/OptimizationPasses/CopyProp.h"

void PassManager::optimize(int level)
{
  for (int i = 0; i < 1; i++)
  {
    for (auto &bbContView : fileView.bbContainerViews)
    {
      {
        DataflowSolver<ConstantsAtStmt> constantsAtStmtSolver(bbContView.cfgManager, true, [&]()
                                                              { return ConstantsAtStmt::bottom(); });
        for (auto &e : constantsAtStmtSolver.run(ConstantsAtStmt::boundary()))
        {
          ConstantProp::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
      }

      {
        auto uncapturedStackBindings = bbContView.getUncapturedStackBindings();
        DataflowSolver<TDZA> tdzaSolver(bbContView.cfgManager, true, [&]()
                                        { return TDZA::bottom(uncapturedStackBindings); });
        for (auto &e : tdzaSolver.run(TDZA::boundary(uncapturedStackBindings)))
        {
          WriteBarrierReduction::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
      }

      // {
      //   std::set<std::shared_ptr<EnvBindingSEXP>> capturedStackBindings = bbContView.getCapturedStackBindings();
      //   DataflowSolver<Liveness> livenessSolver(bbContView.cfgManager.cfg, bbContView.cfgManager.entryBlock(), false, [&]() {
      //     return Liveness::bottom();
      //   });
      //   auto res = livenessSolver.run(Liveness::boundary(capturedStackBindings));
      // }

      {
        std::set<std::shared_ptr<EnvBindingSEXP>> capturedStackBindings = bbContView.getCapturedStackBindings();
        CopyPropInfo::blacklist = capturedStackBindings;
        DataflowSolver<CopyPropInfo> copyPropInfoSolver(bbContView.cfgManager, true, [&]()
                                                        { return CopyPropInfo(); });

        for (auto &e : copyPropInfoSolver.run(CopyPropInfo()))
        {
          auto currBB = bbContView.cfgManager.cfg[e.first];
          
          // std::cout << "BB(" << currBB->getIDX() << "):" << std::endl;
          // e.second.iter(currBB, [&](size_t idx, CopyPropInfo dfVal) {
          //   dfVal.dump(std::cout);
          //   currBB->args.at(idx)->prettyPrint(std::cout, 2);
          //   std::cout << std::endl;
          // });

          // std::cout << std::endl;

          CopyProp::Transform(currBB, e.second);
        }
      }

    }
    {
      fileView.refreshSymbolTable();
      doUnreadBindingRemoval(fileView, iridiumBuildContext);
    }
  }
}

std::shared_ptr<FileSEXP> PassManager::checkout()
{
  return fileView.checkout();
}