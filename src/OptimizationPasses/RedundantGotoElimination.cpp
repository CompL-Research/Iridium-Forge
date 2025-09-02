#include "Iridium/OptimizationPasses/RedundantGotoElimination.h"
//
// 1. Redundant goto elimination / BB merge
//
//   [ BB1 ] --> [ BB2 ]
//
//   REDUNDANT_GOTO_ELIM(BB1, BB2):
//      ∧ BB2 ∈ SUCC(BB1)
//      ∧ BB1 ∈ PRED(BB2)
//      ∧ |SUCC(BB1)| = 1
//      ∧ |PRED(BB2)| = 1
//      ∧ BB1' =  [...INST(BB1), ...INST(BB2)]
//      ∧ SUCC(BB1') = SUCC(BB2)
//      ∧ (∀p ∈ PRED(BB1). ADD_SUCC(p, BB1'). REMOVE_SUCC(p, BB1))
//
//    Note: Traversal of CFG basically guarantees the first two predicates to hold trivially.
//

void doRedundantGotoElimination(FileView &fv)
{
  auto &bbContainerViews = fv.getBBContainerViews();

  for (auto &view : bbContainerViews)
  {
    auto &cfgManager = view.cfgManager;
    std::set<std::shared_ptr<BBSEXP>> toRemove;
    cfgManager.traverseCFG(
        [&](Vertex v, std::shared_ptr<BBSEXP> bb)
        {
          auto succs = cfgManager.successors(v);
          if (succs.size() == 1)
          {
            Vertex s = succs[0];
            if (cfgManager.predecessors(s).size() == 1)
            {
              // std::cout << "mergeSequentialBlocks" << std::endl;
              cfgManager.mergeSequentialBlocks(v, s);
              toRemove.insert(cfgManager.cfg[s]);
            }
          }
        });
    
    auto targetContainer = cfgManager.targetContainer;
    for (auto & bbToRem : toRemove)
    {
      auto it = std::find(targetContainer->args.begin(), targetContainer->args.end(), bbToRem);
      assert(it != targetContainer->args.end());
      size_t index = std::distance(targetContainer->args.begin(), it);

      targetContainer->args.erase(targetContainer->args.begin() + index);
    }
    view.populateSymbolTable();
    view.initCFG();
  }
}