#include "Iridium/OptimizationPasses/UnreadBindingRemoval.h"
#include "Iridium/CorePasses/5_initializeStackFrame.h"
// 
// 2. Unread Binding Removal
// 
//    DEAD_STORE_REMOVAL(BDUChain):
//      ∧ ∀s ∈ BDUChain.localBindings
//        ∧ |s| = 1
// 

void doUnreadBindingRemoval(FileView &fv)
{
  auto & symbolTable = fv.getSymbolTable();
  std::set<IRISEXP> bindingsToDelete;
  std::set<IRISEXP> remoteBindingsToDelete;
  for (auto & b : symbolTable)
  {
    auto & symbolMetadata = b.second;
    if (symbolMetadata.localReads.size() == 0 && symbolMetadata.remoteReads.size() == 0)
    {
      if (symbolMetadata.isTopLevelModuleBinding) remoteBindingsToDelete.insert(b.first);
      else bindingsToDelete.insert(b.first);
    }
  }

}