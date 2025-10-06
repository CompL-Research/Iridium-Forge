#include "Iridium/OptimizationPasses/DeadBindingRemoval.h"

//
// Dead Binding Removal
//  Bindings which are never read from or written to can be safely removed.
//

void doDeadBindingRemoval(FileView &fv)
{
  
  fv.refreshSymbolTable();

  for (auto &bbCont : fv.bbContainerViews)
  {
    if (bbCont.tainted) continue;
    for (auto & b : bbCont.bindingsView.bindings)
    {
      for (auto it = b.second.begin(); it != b.second.end();)
      {
        auto currBinding = *it;
        assert(fv.symbolTable.count(currBinding));
        auto &symbolMetadata = fv.symbolTable[currBinding];
        if (symbolMetadata.localReads.size() == 0 && symbolMetadata.remoteReads.size() == 0 && symbolMetadata.localWrites.size() == 0 && symbolMetadata.remoteWrites.size() == 0)
        {
          fv.symbolTable.erase(currBinding); // safe, gives next iterator
          it = b.second.erase(it);
        }
        else
        {
          ++it;
        }
      }
    }
  }
}