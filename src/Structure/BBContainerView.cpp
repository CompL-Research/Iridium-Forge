#include "Iridium/Globals.h"
#include "Iridium/Structure/BBContainerView.h"
#include <boost/graph/graphviz.hpp>

BBContainerView::BBContainerView(std::shared_ptr<BBContainerSEXP> target) : targetContainer(target)
{
  initBDUChains();
  initCFG();
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
    assert(currBinding);
    auto predicate = [&](const IRISEXP &ele)
    {
      return ele == b;
    };

    // Collect stmts that contain a super() constructor call
    for (auto &bb : bbList->args)
    {
      auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);
      assert(currBB);

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

void BBContainerView::initCFG()
{
  auto startBBIdx = targetContainer->getStartBBIDX();
  auto bbList = std::dynamic_pointer_cast<ListSEXP>(targetContainer->getBB());
  assert(bbList && "bbList is null");

  // Add all BBs as vertices to the control flow graph
  for (auto &bb : bbList->args)
  {
    auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);
    assert(currBB && "currBB is null 1");
    cfgManager.addNode(currBB, currBB->getIDX() == startBBIdx);
  }

  // Add all BBs as vertices to the control flow graph
  for (auto &bb : bbList->args)
  {
    auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);
    assert(currBB && "currBB is null 2");
    auto sourceBBIdx = currBB->getIDX();

    auto lastStmt = currBB->args.back();
    
    if (auto gotoStmt = std::dynamic_pointer_cast<GotoSEXP>(lastStmt))
    {
      double targetBBIdx = gotoStmt->getIDX();
      cfgManager.connect(sourceBBIdx, targetBBIdx);
    }
    else if (auto ifElseStmt = std::dynamic_pointer_cast<IfElseJumpSEXP>(lastStmt))
    {
      double targetTrueIdx = ifElseStmt->getTRUE();
      double targetFalseIdx = ifElseStmt->getFALSE();

      cfgManager.connect(sourceBBIdx, targetTrueIdx);
      cfgManager.connect(sourceBBIdx, targetFalseIdx);
      
    }
    else if (auto invokeFinalizer = std::dynamic_pointer_cast<InvokeFinalizerSEXP>(lastStmt))
    {
    }
    else if (auto returnStmt = std::dynamic_pointer_cast<ReturnSEXP>(lastStmt))
    {
    }
    else if (auto returnAsyncStmt = std::dynamic_pointer_cast<ReturnAsyncSEXP>(lastStmt))
    {
    }
    else if (auto returnStmt = std::dynamic_pointer_cast<RetSEXP>(lastStmt))
    {
      // TODO, retSEXP
    }
    else
    {
      throw std::runtime_error("Unexpected LastNode in BB: " + lastStmt->tag);
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

std::string escapeDotLabel(const std::string &s)
{
  std::string out;
  out.reserve(s.size());
  for (char c : s)
  {
    switch (c)
    {
    case '"':
      out += "\\\"";
      break;
    case '\\':
      out += "\\\\";
      break;
    case '\n':
      out += "\\l";
      break; // left aligned line breaks
    default:
      out += c;
      break;
    }
  }
  out += "\\l"; // final \l so last line left-aligns too
  return out;
}

void CFGManager::dumpCFGDOT(std::string filePath)
{
  // Write to DOT file
  std::ofstream dot_file(filePath);

  boost::write_graphviz(
      dot_file, cfg,
      // Lambda to print custom vertex properties
      [&](std::ostream &out, auto v)
      {
        std::stringstream ss;
        cfg[v]->prettyPrint(ss);
        out << "[label=\"BB" << cfg[v]->getIDX() << "\\l"
            << escapeDotLabel(ss.str())
            << "\", shape=box, style=rounded, fontname=\"Courier\", fontsize=10]";
      });
}