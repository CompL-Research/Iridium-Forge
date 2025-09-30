#include "Iridium/Structure/FileView.h"


void FileView::init()
{
  while (true)
  {
    auto bbContainer = std::dynamic_pointer_cast<BBContainerSEXP>(targetContainer->args.back());
    if (!bbContainer) break;
    bbContainerViews.push_back(BBContainerView(bbContainer, symbolTable, iridiumBuildContext));
    targetContainer->args.pop_back(); // Remove the container completely while it is owned by the view.
  }
}

void FileView::dumpSymbolTable(std::ostringstream &oss)
{
  oss << "== Symbol Table ==" << std::endl;
  for (auto & b : symbolTable)
  {
    b.first->dump(oss, true, 2);
    oss << std::endl;
    b.second.dump(oss, false, 2);
  }
}

void FileView::refreshSymbolTable()
{
  symbolTable.clear();
  for (auto &bbCont : bbContainerViews)
  {
    bbCont.populateSymbolTable();
  }
}

void FileView::deleteBinding(std::shared_ptr<EnvBindingSEXP> binding)
{
  for (auto &bbCont : bbContainerViews)
  {
    bbCont.bindingsView.removeBinding(binding);
  }
}

std::shared_ptr<BBSEXP> FileView::getBB(SEXPPath path)
{
  for (auto & bbView : bbContainerViews)
  {
    if (bbView.getScopeIdx() == path.scopeIdx)
    {
      for (auto & bb : bbView.targetContainer->getBB()->args)
      {
        if (auto bbSEXP = std::dynamic_pointer_cast<BBSEXP>(bb))
        {
          if (bbSEXP->getIDX() == path.bbIdx) return bbSEXP;
        } else throw std::runtime_error("Expected BBSEXP");
        
      }
    }
  }

  throw std::runtime_error("getBB failed!!!");
}

std::shared_ptr<FileSEXP> FileView::checkout()
{
  refreshSymbolTable();
  

  for (auto & v : bbContainerViews)
  {
    targetContainer->args.push_back(v.checkout());
  }

  return targetContainer;
}


void FileView::updateSafelyCapturedBindingsSet(double bbIDX, TDZA val)
{
  for (auto & targetContainer : bbContainerViews)
  {
    if (targetContainer.getStartBBIDX() == bbIDX)
    {
      std::shared_ptr<ListSEXP> remoteBindingsList = std::dynamic_pointer_cast<ListSEXP>(targetContainer.bindingsView.remoteBindings);
      assert(remoteBindingsList);

      for (auto & b : remoteBindingsList->args)
      {
        std::shared_ptr<ListSEXP> remoteBindingsList = std::dynamic_pointer_cast<ListSEXP>(targetContainer.bindingsView.remoteBindings);
        assert(remoteBindingsList);

        for (auto & r : remoteBindingsList->args)
        {
          std::shared_ptr<RemoteEnvBindingSEXP> rBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(r);
          assert(rBinding);

          std::shared_ptr<EnvBindingSEXP> resolvedBinding = resolveRemoteBinding(rBinding);

          if (val.dfv.store.count(resolvedBinding) > 0 && val.dfv.store[resolvedBinding].kind == TDZLattice::SAFE)
          {
            safelyCapturedBindings.insert(rBinding);
          }
        }
      }
    }
  }
}