#include "Iridium/Structure/FileView.h"

void FileView::init()
{
  for (auto &c : targetContainer->args)
  {
    if (auto bbContainer = std::dynamic_pointer_cast<BBContainerSEXP>(c))
    {
      bbContainerViews.push_back(BBContainerView(bbContainer, symbolTable));
    }
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

std::shared_ptr<BBSEXP> FileView::getBB(double scopeIDX, double bbIDX)
{
  for (auto & bbView : bbContainerViews)
  {
    if (bbView.scopeIdx == scopeIDX)
    {
      for (auto & bb : bbView.targetContainer->getBB()->args)
      {
        if (auto bbSEXP = std::dynamic_pointer_cast<BBSEXP>(bb))
        {
          if (bbSEXP->getIDX() == bbIDX) return bbSEXP;
        } else throw std::runtime_error("Expected BBSEXP");
        
      }
    }
  }

  throw std::runtime_error("getBB failed!!!");
}