#include "Iridium/OptimizationPasses/UnreadBindingRemoval.h"
#include "Iridium/CorePasses/5_initializeStackFrame.h"
//
// 2. Unread Binding Removal
//
//    DEAD_STORE_REMOVAL(BDUChain):
//      ∧ ∀s ∈ BDUChain.localBindings
//        ∧ |s| = 1
//

void doUnreadBindingRemoval(FileView &fv, std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext)
{

  bool changed = false;
  do
  {
    changed = false;
    auto &symbolTable = fv.symbolTable;
    for (auto it = symbolTable.begin(); it != symbolTable.end();)
    {
      auto &symbolMetadata = it->second;
      if (symbolMetadata.localReads.size() == 0 && symbolMetadata.remoteReads.size() == 0)
      {
        changed = true;
        // Delete the binding
        fv.deleteBinding(symbolMetadata.binding);

        std::vector<SEXPPath> stmts;
        stmts.reserve(symbolMetadata.localWrites.size() + symbolMetadata.remoteWrites.size());

        stmts.insert(stmts.end(),
                     symbolMetadata.localWrites.begin(),
                     symbolMetadata.localWrites.end());

        stmts.insert(stmts.end(),
                     symbolMetadata.remoteWrites.begin(),
                     symbolMetadata.remoteWrites.end());

        for (auto &path : stmts)
        {
          auto bbSEXP = fv.getBB(path);
          auto it = std::find(bbSEXP->args.begin(), bbSEXP->args.end(), path.inst);
          assert(it != bbSEXP->args.end());
          size_t index = std::distance(bbSEXP->args.begin(), it);

          if (auto curr = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(path.inst))
          {
            bbSEXP->args.erase(bbSEXP->args.begin() + index);
          }
          else if (auto curr = std::dynamic_pointer_cast<EnvWriteSEXP>(path.inst))
          {
            if (std::dynamic_pointer_cast<JSNUBDSEXP>(curr->getRVal()))
            {
              bbSEXP->args.erase(bbSEXP->args.begin() + index);
            }
            else if (auto innerRVal = std::dynamic_pointer_cast<EnvWriteSEXP>(curr->getRVal()))
            {
              bbSEXP->args.at(index) = curr->getRVal();
            }
            else
            {
              bbSEXP->args.at(index) = std::make_shared<StackRejectSEXP>(1);
              bbSEXP->args.at(index)->args.push_back(curr->getRVal());
            }
          }
          else
            throw std::runtime_error("Invalid env write...");
        }
        it = symbolTable.erase(it); // safe, gives next iterator
      }
      else
      {
        ++it;
      }
    }
    fv.refreshSymbolTable();
  } while(changed);
}