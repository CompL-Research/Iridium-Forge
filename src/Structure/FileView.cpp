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