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
  auto &symbolTable = fv.getSymbolTable();
  std::vector<SymbolMetadata> bindingsToDelete;
  for (auto &b : symbolTable)
  {
    auto &symbolMetadata = b.second;
    if (symbolMetadata.localReads.size() == 0 && symbolMetadata.remoteReads.size() == 0)
    {
      bindingsToDelete.push_back(symbolMetadata);
    }
  }

  for (auto &btd : bindingsToDelete)
  {
    for (auto &path : btd.localWrites)
    {
      auto bbSEXP = fv.getBB(path.scopeIdx, path.bbIdx);
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
        else
        {
          bbSEXP->args.at(index) = std::make_shared<StackRejectSEXP>(1);
          bbSEXP->args.at(index)->args.push_back(curr->getRVal());
        }
      }
      else
        throw std::runtime_error("Invalid env write...");
    }

    for (auto &path : btd.remoteWrites)
    {
      auto bbSEXP = fv.getBB(path.scopeIdx, path.bbIdx);
      auto it = std::find(bbSEXP->args.begin(), bbSEXP->args.end(), path.inst);
      assert(it != bbSEXP->args.end());
      size_t index = std::distance(bbSEXP->args.begin(), it);

      if (auto curr = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(path.inst))
      {
        bbSEXP->args.erase(bbSEXP->args.begin() + index);
      }
      else if (auto curr = std::dynamic_pointer_cast<EnvWriteSEXP>(path.inst))
      {
        bbSEXP->args.at(index) = std::make_shared<StackRejectSEXP>(1);
        bbSEXP->args.at(index)->args.push_back(curr->getRVal());
      }
      else
        throw std::runtime_error("Invalid env write...");
    }

    auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(btd.frame->getBindings());
    assert(bindingsSEXP);

    if (btd.isTopLevelModuleBinding)
    {

      auto remoteBindingsSEXP = bindingsSEXP->getRemoteBindings();
      int index = -1;
      size_t i = 0;
      for (auto &b : remoteBindingsSEXP->args)
      {

        auto currRemoteBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b);
        assert(currRemoteBinding);
        auto currBinding = resolveRemoteBinding(currRemoteBinding);
        assert(currBinding);
        if (currBinding == btd.binding)
        {
          index = i;
          break;
        }
        i++;
      }
      assert(index > -1);
      remoteBindingsSEXP->args.erase(remoteBindingsSEXP->args.begin() + index);
    }
    else
    {
      auto localBindingsSEXP = bindingsSEXP->getLocalBindings();
      int index = -1;
      size_t i = 0;
      for (auto &b : localBindingsSEXP->args)
      {

        auto currBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(b);
        assert(currBinding);
        if (b == btd.binding)
        {
          index = i;
          break;
        }
        i++;
      }
      assert(index > -1);
      localBindingsSEXP->args.erase(localBindingsSEXP->args.begin() + index);
    }

    std::cout << "Removing binding: " << btd.binding->getNAME() << std::endl;

    rebalanceStackFrame(btd.frame, iridiumBuildContext, btd.isTopLevelModuleBinding);
    symbolTable.erase(btd.binding);
  }
}