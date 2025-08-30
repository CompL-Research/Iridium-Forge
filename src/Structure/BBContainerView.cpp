#include "Iridium/Globals.h"
#include "Iridium/Structure/BBContainerView.h"

BBContainerView::BBContainerView(std::shared_ptr<BBContainerSEXP> target) : targetContainer(target)
{
  initBDUChains();
}

void BBContainerView::initBDUChains()
{
  auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(targetContainer->getBindings());
  assert(bindingsSEXP);
  auto bbList = std::dynamic_pointer_cast<ListSEXP>(targetContainer->getBB());
  assert(bbList);
  for (auto &b : bindingsSEXP->getLocalBindings()->args)
  {

    auto currBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(b);

    auto predicate = [&](const IRISEXP &ele)
    {
      return ele == b;
    };

    // Collect stmts that contain a super() constructor call
    for (auto &bb : bbList->args)
    {
      auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);

      for (auto &stmt : currBB->args)
      {
        if (hasNode(stmt, predicate))
        {
          addBDUStmtElement(currBinding, stmt);
        }
      }
    }
  }

  // Top level scopes in modules make these bindings, in case of a script this will be empty,
  // as all top level declaration becoma part of the global object...
  if (targetContainer->hasTopLevel())
  {
    for (auto &b : bindingsSEXP->getRemoteBindings()->args)
    {

      auto currBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b);

      auto predicate = [&](const IRISEXP &ele)
      {
        return ele == b;
      };

      // Collect stmts that contain a super() constructor call
      for (auto &bb : bbList->args)
      {
        auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);

        for (auto &stmt : currBB->args)
        {
          if (hasNode(stmt, predicate))
          {
            addBDUStmtElement(currBinding, stmt);
          }
        }
      }
    }
  }
}

void BBContainerView::addBDUStmtElement(std::shared_ptr<EnvBindingSEXP> binding, IRISEXP stmt)
{
  bDUStmtMap[binding].push_back(stmt);
}

void BBContainerView::addBDUStmtElement(std::shared_ptr<RemoteEnvBindingSEXP> binding, IRISEXP stmt)
{
  bDUStmtMapTopLevelModuleBindings[binding].push_back(stmt);
}

void BBContainerView::dumpBDUChains(std::ostringstream &oss, bool compressed, int indent)
{
  oss << "[BDUChains]" << std::endl;

  for (auto &e : bDUStmtMap)
  {
    oss << "  Binding = " << e.first->getNAME() << std::endl;
    for (int i = 0; i < e.second.size(); i++)
    {
      oss << "    " << i << std::endl;
      e.second.at(i)->dump(oss, compressed, 6);
      oss << std::endl;
    }
  }

  for (auto &e : bDUStmtMapTopLevelModuleBindings)
  {
    
    oss << "  Binding = " << resolveRemoteBinding(e.first)->getNAME() << std::endl;
    for (int i = 0; i < e.second.size(); i++)
    {
      oss << "    " << i << std::endl;
      e.second.at(i)->dump(oss, compressed, 6);
      oss << std::endl;
    }
  }
}