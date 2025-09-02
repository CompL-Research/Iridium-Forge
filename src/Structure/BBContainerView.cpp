#include "Iridium/Globals.h"
#include "Iridium/Structure/BBContainerView.h"
#include <boost/graph/graphviz.hpp>

BBContainerView::BBContainerView(std::shared_ptr<BBContainerSEXP> target, SymbolTable &symbolTable) : targetContainer(target), symbolTable(symbolTable)
{
  populateSymbolTable();
  initCFG();
}

void BBContainerView::populateSymbolTable()
{
  auto bindingsSEXP = std::dynamic_pointer_cast<BindingsSEXP>(targetContainer->getBindings());
  assert(bindingsSEXP);
  for (auto &b : bindingsSEXP->getLocalBindings()->args)
  {
    auto currBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(b);
    assert(currBinding);
    if (symbolTable.find(currBinding) == symbolTable.end())
      symbolTable[currBinding] = SymbolMetadata();
  }

  if (targetContainer->hasTopLevel())
  {
    for (auto &b : bindingsSEXP->getRemoteBindings()->args)
    {
      auto currRemoteBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b);
      assert(currRemoteBinding);
      auto currBinding = resolveRemoteBinding(currRemoteBinding);
      assert(currBinding);
      if (symbolTable.find(currBinding) == symbolTable.end())
        symbolTable[currBinding] = SymbolMetadata();
    }
  }

  // For each binding, create an entry in the symbol table
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
    // Collect stmts that contain are using this binding
    for (auto &bb : bbList->args)
    {
      auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);
      assert(currBB);
      for (auto &stmt : currBB->args)
      {
        if (hasNode(stmt, predicate))
        {
          SEXPPath path = {targetContainer->getScopeIDX(), currBB->getIDX(), stmt};
          if (auto implicitBindingDecl = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(stmt))
          {
            if (implicitBindingDecl->getStore() == currBinding)
              symbolTable[currBinding].localWrites.push_back(path);
            else
              symbolTable[currBinding].localReads.push_back(path);
          }
          else if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
          {
            if (envWrite->getLValTarget() == currBinding)
              symbolTable[currBinding].localWrites.push_back(path);
            else
              symbolTable[currBinding].localReads.push_back(path);
          }
          else
          {
            symbolTable[currBinding].localReads.push_back(path);
          }
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
      auto currRemoteBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b);
      assert(currRemoteBinding);
      auto currBinding = resolveRemoteBinding(currRemoteBinding);
      assert(currBinding);
      auto predicate = [&](const IRISEXP &ele)
      {
        return ele == b;
      };

      symbolTable[currBinding].isTopLevelModuleBinding = true;

      // Collect stmts that contain are using this binding
      for (auto &bb : bbList->args)
      {
        auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);
        assert(currBB);
        for (auto &stmt : currBB->args)
        {
          if (hasNode(stmt, predicate))
          {
            SEXPPath path = {targetContainer->getScopeIDX(), currBB->getIDX(), stmt};
            if (auto implicitBindingDecl = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(stmt))
            {
              if (implicitBindingDecl->getStore() == currBinding)
                symbolTable[currBinding].localWrites.push_back(path);
              else
                symbolTable[currBinding].localReads.push_back(path);
            }
            else if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
            {
              if (envWrite->getLValTarget() == currBinding)
              {
                symbolTable[currBinding].localWrites.push_back(path);
              }
              else if (auto remoteTargetWrite = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(envWrite->getLValTarget()))
              {
                if (resolveRemoteBinding(remoteTargetWrite) == currBinding) symbolTable[currBinding].localWrites.push_back(path);
                else symbolTable[currBinding].localReads.push_back(path);
              }
              else
                symbolTable[currBinding].localReads.push_back(path);
            }
            else
            {
              symbolTable[currBinding].localReads.push_back(path);
            }
          }
        }
      }
    }
  }
  else
  {
    for (auto &b : bindingsSEXP->getRemoteBindings()->args)
    {
      auto currRemoteBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(b);
      assert(currRemoteBinding);
      auto currBinding = resolveRemoteBinding(currRemoteBinding);
      assert(currBinding);
      auto predicate = [&](const IRISEXP &ele)
      {
        return ele == b;
      };
      // Collect stmts that contain are using this binding
      for (auto &bb : bbList->args)
      {
        auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);
        assert(currBB);
        for (auto &stmt : currBB->args)
        {
          if (hasNode(stmt, predicate))
          {
            SEXPPath path = {targetContainer->getScopeIDX(), currBB->getIDX(), stmt};
            if (auto implicitBindingDecl = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(stmt))
            {
              if (implicitBindingDecl->getStore() == currBinding)
                symbolTable[currBinding].remoteWrites.push_back(path);
              else
                symbolTable[currBinding].remoteReads.push_back(path);
            }
            else if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
            {
              if (envWrite->getLValTarget() == currBinding)
                symbolTable[currBinding].remoteWrites.push_back(path);
              else
                symbolTable[currBinding].remoteReads.push_back(path);
            }
            else
            {
              symbolTable[currBinding].remoteReads.push_back(path);
            }
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