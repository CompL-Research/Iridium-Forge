#include "Iridium/Globals.h"
#include "Iridium/Structure/BBContainerView.h"
#include "Iridium/Structure/RegisterAllocator.h"
#include <boost/graph/graphviz.hpp>
#include <filesystem>
#include "external/json.hpp"

#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/DataflowSolver.h"
#include "Iridium/Structure/FileView.h"

using json = nlohmann::json;

BBContainerView::BBContainerView(
    std::shared_ptr<BBContainerSEXP> target,
    SymbolTable &symbolTable,
    std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext) : targetContainer(target),
                                                                     symbolTable(symbolTable),
                                                                     iridiumBuildContext(iridiumBuildContext),
                                                                     cfgManager(iridiumBuildContext),
                                                                     bindingsView(targetContainer->getScopeIDX(), targetContainer->getBindings(), iridiumBuildContext)
{
  populateSymbolTable();
  initCFG();
}

void BBContainerView::refreshSymbolTable()
{
  for (auto &b : bindingsView.bindings)
  {
    for (auto &currBinding : b.second)
    {
      if (symbolTable.find(currBinding) == symbolTable.end())
        symbolTable[currBinding] = SymbolMetadata();
      symbolTable[currBinding].binding = currBinding;
    }
  }

  if (targetContainer->hasTopLevel())
  {
    for (auto &currRemoteBinding : bindingsView.remoteBindings)
    {
      auto currBinding = resolveRemoteBinding(currRemoteBinding);
      assert(currBinding);
      if (symbolTable.find(currBinding) == symbolTable.end())
        symbolTable[currBinding] = SymbolMetadata();
      symbolTable[currBinding].binding = currBinding;
      symbolTable[currBinding].isTopLevelModuleBinding = true;
    }
  }

  // For each binding, create an entry in the symbol table
  for (auto &b : bindingsView.bindings)
  {
    for (auto &currBinding : b.second)
    {
      if (FileView::dynamicEvaledBindings.count(currBinding) > 0)
      {
        SEXPPath path = {-1, -1, std::make_shared<NullSEXP>(true)};
        symbolTable[currBinding].remoteReads.push_back(path);
        symbolTable[currBinding].remoteWrites.push_back(path);
      }
      auto predicate = [&](const IRISEXP &ele)
      {
        return ele == currBinding;
      };

      cfgManager.traverseCFG(
          [&](Vertex v, std::shared_ptr<BBSEXP> currBB)
          {
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
          });
    }
  }

  for (auto &currRemoteBinding : bindingsView.remoteBindings)
  {
    auto currBinding = resolveRemoteBinding(currRemoteBinding);
    assert(currBinding);
    auto predicate = [&](const IRISEXP &ele)
    {
      return ele == currRemoteBinding;
    };

    auto &readsVector = targetContainer->hasTopLevel() ? symbolTable[currBinding].localReads : symbolTable[currBinding].remoteReads;
    auto &writesVector = targetContainer->hasTopLevel() ? symbolTable[currBinding].localWrites : symbolTable[currBinding].remoteWrites;

    cfgManager.traverseCFG(
        [&](Vertex v, std::shared_ptr<BBSEXP> currBB)
        {
          for (auto &stmt : currBB->args)
          {
            if (hasNode(stmt, predicate))
            {
              SEXPPath path = {targetContainer->getScopeIDX(), currBB->getIDX(), stmt};
              if (auto implicitBindingDecl = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(stmt))
              {
                if (implicitBindingDecl->getStore() == currRemoteBinding)
                  writesVector.push_back(path);
                else
                  readsVector.push_back(path);
              }
              else if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
              {
                if (envWrite->getLValTarget() == currRemoteBinding)
                  writesVector.push_back(path);
                else
                  readsVector.push_back(path);
              }
              else
              {
                readsVector.push_back(path);
              }
            }
          }
        });
  }
}

void BBContainerView::populateSymbolTable()
{

  for (auto &b : bindingsView.bindings)
  {
    for (auto &currBinding : b.second)
    {
      if (symbolTable.find(currBinding) == symbolTable.end())
        symbolTable[currBinding] = SymbolMetadata();
      symbolTable[currBinding].binding = currBinding;
    }
  }

  if (targetContainer->hasTopLevel())
  {
    for (auto &currRemoteBinding : bindingsView.remoteBindings)
    {
      auto currBinding = resolveRemoteBinding(currRemoteBinding);
      assert(currBinding);
      if (symbolTable.find(currBinding) == symbolTable.end())
        symbolTable[currBinding] = SymbolMetadata();
      symbolTable[currBinding].binding = currBinding;
      symbolTable[currBinding].isTopLevelModuleBinding = true;
    }
  }

  // For each binding, create an entry in the symbol table
  auto bbList = std::dynamic_pointer_cast<ListSEXP>(targetContainer->getBB());
  assert(bbList);
  for (auto &b : bindingsView.bindings)
  {
    for (auto &currBinding : b.second)
    {

      if (FileView::dynamicEvaledBindings.count(currBinding) > 0)
      {
        SEXPPath path = {-1, -1, std::make_shared<NullSEXP>(true)};
        symbolTable[currBinding].remoteReads.push_back(path);
        symbolTable[currBinding].remoteWrites.push_back(path);
      }
      
      auto predicate = [&](const IRISEXP &ele)
      {
        return ele == currBinding;
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
  }

  for (auto &currRemoteBinding : bindingsView.remoteBindings)
  {
    auto currBinding = resolveRemoteBinding(currRemoteBinding);
    assert(currBinding);
    auto predicate = [&](const IRISEXP &ele)
    {
      return ele == currRemoteBinding;
    };

    auto &readsVector = targetContainer->hasTopLevel() ? symbolTable[currBinding].localReads : symbolTable[currBinding].remoteReads;
    auto &writesVector = targetContainer->hasTopLevel() ? symbolTable[currBinding].localWrites : symbolTable[currBinding].remoteWrites;

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
            if (implicitBindingDecl->getStore() == currRemoteBinding)
              writesVector.push_back(path);
            else
              readsVector.push_back(path);
          }
          else if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
          {
            if (envWrite->getLValTarget() == currRemoteBinding)
              writesVector.push_back(path);
            else
              readsVector.push_back(path);
          }
          else
          {
            readsVector.push_back(path);
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

  cfgManager.targetContainer = bbList;

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

    // Inside a BB, if there is a premature control stmt => due to lazily resolved resolved [continue] / [break] target, the rest of the code in the basic block becomes unreachable.

    for (auto &lastStmt : currBB->args)
    {
      if (auto gotoStmt = std::dynamic_pointer_cast<GotoSEXP>(lastStmt))
      {
        double targetBBIdx = gotoStmt->getIDX();
        cfgManager.connect(sourceBBIdx, targetBBIdx, {EdgeKind::Normal, gotoStmt});
      }
      else if (auto ifElseStmt = std::dynamic_pointer_cast<IfElseJumpSEXP>(lastStmt))
      {
        double targetTrueIdx = ifElseStmt->getTRUE();
        double targetFalseIdx = ifElseStmt->getFALSE();

        cfgManager.connect(sourceBBIdx, targetTrueIdx, {EdgeKind::Normal, ifElseStmt});
        cfgManager.connect(sourceBBIdx, targetFalseIdx, {EdgeKind::Normal, ifElseStmt});
      }
      else if (auto invokeFinalizer = std::dynamic_pointer_cast<InvokeFinalizerSEXP>(lastStmt))
      {
        cfgManager.connect(sourceBBIdx, invokeFinalizer->getIDX(), {EdgeKind::Finalizer, invokeFinalizer});
        continue;
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
      else if (auto throwStmt = std::dynamic_pointer_cast<ThrowSEXP>(lastStmt))
      {
        // TODO, retSEXP
      }
      else
      {
        continue;
      }

      // If this is indeed a inst which exists out of the basic block, delete the rest of the instructions...
      auto it = std::find(currBB->args.begin(), currBB->args.end(), lastStmt);
      if (it != currBB->args.end())
      {
        currBB->args.erase(it + 1, currBB->args.end());
        break;
      }
    }
  }

  std::function<std::vector<Vertex>(const CFG &g, Vertex start)> findSinks = [&](const CFG &g, Vertex start)
  {
    std::vector<Vertex> sinks;
    std::set<Vertex> visited;

    std::function<void(Vertex)> dfs = [&](Vertex u)
    {
      if (!visited.insert(u).second)
        return;

      auto deg = boost::out_degree(u, g);
      if (deg == 0)
      {
        sinks.push_back(u);
        return;
      }

      auto [ei, ei_end] = boost::out_edges(u, g);
      for (; ei != ei_end; ++ei)
      {
        Vertex tgt = boost::target(*ei, g);
        dfs(tgt);
      }
    };

    dfs(start);
    return sinks;
  };

  // Add back edges from finalizer BB to all its invocation sites
  for (auto &bb : bbList->args)
  {
    auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);
    assert(currBB && "currBB is null 1");

    auto &tryContext = iridiumBuildContext[currBB->getScopeIDX()]->tryContext;
    if (tryContext)
    {
      // Add edges from all finalizer sinks to their respective call sites
      auto &finalizerIdx = tryContext.value().finalizerIDX;
      if (finalizerIdx > -1)
      {
        assert(cfgManager.bbIdxToVertex.find(finalizerIdx) != cfgManager.bbIdxToVertex.end());
        auto finalizerVertex = cfgManager.bbIdxToVertex[finalizerIdx];
        auto incoming = cfgManager.predecessors(finalizerVertex);
        auto sinks = findSinks(cfgManager.cfg, finalizerVertex);

        for (auto &sink : sinks)
        {
          for (auto &invocationSite : incoming)
          {
            cfgManager.connect(sink, invocationSite, {EdgeKind::FinalizerRet, NULL});
          }
        }
      }
    }
  }

  // Add edges from all nested Try blocks to their respective udCatchBB or imCatchBB
  for (auto &bb : bbList->args)
  {
    auto currBB = std::dynamic_pointer_cast<BBSEXP>(bb);
    assert(currBB && "currBB is null X");
    auto context = maybeGetEnclosingTryCatchContext(currBB->getScopeIDX(), iridiumBuildContext);
    if (context)
    {
      auto tryContext = context->tryContext.value();
      // Path from tryBB
      if (hasScopePath(currBB->getScopeIDX(), getBBScopeIDX(tryContext.tryIDX, iridiumBuildContext), iridiumBuildContext))
      {
        if (tryContext.udCatchIDX > -1)
        {
          cfgManager.connect(currBB->getIDX(), tryContext.udCatchIDX, {EdgeKind::Exception, NULL});
        }
        else
        {
          assert(tryContext.imCatchIDX > -1);
          cfgManager.connect(currBB->getIDX(), tryContext.imCatchIDX, {EdgeKind::Exception, NULL});
        }
      }
      else if (tryContext.udCatchIDX > -1 && hasScopePath(currBB->getScopeIDX(), getBBScopeIDX(tryContext.udCatchIDX, iridiumBuildContext), iridiumBuildContext))
      {
        cfgManager.connect(currBB->getIDX(), tryContext.imCatchIDX, {EdgeKind::Exception, NULL});
      }
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

std::shared_ptr<BBContainerSEXP> BBContainerView::checkout()
{

#if IRIDIUM_DUMP_INITIAL_CFG == 1
  {
    DBG("IRIDIUM_DUMP_INITIAL_CFG");
    std::string savePath = IRIDIUM_OUTPUTS_FOLDER + std::string("Initial_CFG_BB") + std::to_string(this->getStartBBIDX()) + ".DOT";
    std::filesystem::path dir = IRIDIUM_OUTPUTS_FOLDER;
    try
    {
      if (!std::filesystem::exists(dir))
      {
        std::filesystem::create_directories(dir);
      }
      cfgManager.dumpCFGDOT(savePath);
    }
    catch (const std::filesystem::filesystem_error &e)
    {
      std::cerr << "[IRIDIUM_DUMP_INITIAL_CFG] Filesystem error: " << e.what() << '\n';
    }
  }
#endif

  auto capturedStackBindings = getCapturedStackBindings();
  Liveness::blacklist = capturedStackBindings;
  DataflowSolver<Liveness> livenessSolver(cfgManager, false, [&]()
                                          { return Liveness::bottom(); });

  auto livenessResult = livenessSolver.run(Liveness::boundary(capturedStackBindings));

  std::vector<IRISEXP> chapati = cfgManager.chapati();
  std::shared_ptr<ListSEXP> bbList = std::make_shared<ListSEXP>("BB");
  bbList->args = chapati;
  targetContainer->setBB(bbList);
  targetContainer->setBindings(bindingsView.checkout());

#if IRIDIUM_DUMP_BEFORE_STACK_COLLAPSE == 1
  {
    std::string savePath = IRIDIUM_OUTPUTS_FOLDER + std::string("BeforeStackCollapse_") + std::to_string(getStartBBIDX()) + ".iridump";
    std::filesystem::path dir = IRIDIUM_OUTPUTS_FOLDER;
    try
    {
      if (!std::filesystem::exists(dir))
      {
        std::filesystem::create_directories(dir);
      }

      std::ofstream outFile(savePath);
      if (!outFile)
      {
        throw std::runtime_error("Could not open file: " + savePath);
      }
      DBG("IRIDIUM_DUMP_BEFORE_STACK_COLLAPSE");
      targetContainer->prettyPrint(outFile);
      outFile << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
      std::cerr << "[IRIDIUM_DUMP_BEFORE_STACK_COLLAPSE] Filesystem error: " << e.what() << '\n';
    }
  }
#endif

  // if (!tainted) { // Stack collapse
  // bool isClassRelated = targetContainer->getContainerFlagID();
  bool doRegalloc = !tainted;
  // Graph coloring based Reg Alloc fails in presence of dead code...
  if (getenv("NO_OPT") || getenv("NO_WBR") || getenv("NO_EPROP")) doRegalloc = false;

  std::vector<std::shared_ptr<EnvBindingSEXP>> allBindings;
  {
    auto bindingsObj = std::dynamic_pointer_cast<BindingsSEXP>(targetContainer->getBindings());
    assert(bindingsObj);

    for (auto &e : bindingsObj->getLocalBindings()->args)
    {
      auto b = std::dynamic_pointer_cast<EnvBindingSEXP>(e);
      assert(b);
      allBindings.push_back(b);
    }
  }

  if (doRegalloc) {
    std::unique_ptr<RegisterAllocator> regAlloc;
    if (getenv("SCOPE_RA"))
    {
      regAlloc = std::make_unique<ScopeBasedRegisterAllocator>();
    }
    else
    {
      regAlloc = std::make_unique<RAGC>();
    }

    regAlloc->begin(allBindings, capturedStackBindings);

    for (auto &e : livenessResult)
    { // e.second.dfv is a set std::set<std::shared_ptr<EnvBindingSEXP>>
      auto &bb = cfgManager.cfg[e.first];
      auto &boundaryLivenessInfo = e.second;
      boundaryLivenessInfo.iter(bb,
                                [&](size_t idx, const Liveness &val)
                                {
                                  regAlloc->observe(val.dfv);
                                });
    }

    // std::cout << "REGALLOC OP NODES" << std::endl;
    // for (auto & b : regAlloc.nodes)
    // {
    //   b->prettyPrint(std::cout);
    //   std::cout << std::endl;
    // }

    regAlloc->allocate();

    for (auto &cBinding : capturedStackBindings)
    {
      cBinding->setIDX(regAlloc->allocateExtra());
    }

    std::unordered_map<double, std::vector<std::shared_ptr<EnvBindingSEXP>>> stackMap;
    std::vector<IRISEXP> finalStack;

    auto bindingsObj = std::dynamic_pointer_cast<BindingsSEXP>(targetContainer->getBindings());
    assert(bindingsObj);

    for (auto e : bindingsObj->getLocalBindings()->args)
    {
      auto currB = std::dynamic_pointer_cast<EnvBindingSEXP>(e);
      assert(currB);
      if (currB->hasJSARG() || currB->hasJSRESTARG())
      {
        finalStack.push_back(currB);
      }
      else
      {
        stackMap[currB->getREFIDX()].push_back(currB);
      }
    }

    for (auto &e : stackMap)
    {
      if (e.second.size() == 1)
      {
        finalStack.push_back(e.second.back());
      }
      else
      {
        std::stringstream ss;
        for (auto &b : e.second)
        {
          ss << b->getNAME() << " ";
        }
        auto compositeBinding = std::make_shared<EnvBindingSEXP>(ss.str(), false, false, false, false, false, true, -1, e.first, -1, -1, -1);
        finalStack.push_back(compositeBinding);
      }
      // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
    }

    std::sort(finalStack.begin(), finalStack.end(),
              [](const IRISEXP &a, const IRISEXP &b)
              {
                auto a1 = std::dynamic_pointer_cast<EnvBindingSEXP>(a);
                auto b1 = std::dynamic_pointer_cast<EnvBindingSEXP>(b);
                assert(a1);
                assert(b1);
                if (a1->hasJSARG() || b1->hasJSARG())
                {
                  if (a1->hasJSARG() && b1->hasJSARG())
                    return a1->getREFIDX() < b1->getREFIDX();

                  if (a1->hasJSARG())
                    return true;
                  else
                    return false;
                }
                return a1->getREFIDX() < b1->getREFIDX();
              });

    // std::cout << "Stack collapse: " << bindingsObj->getLocalBindings()->args.size() << " -> " << finalStack.size() << std::endl;

    bindingsObj->getLocalBindings()->args = std::move(finalStack);
  }

#if IRIDIUM_DUMP_AFTER_STACK_COLLAPSE == 1
  {
    std::string savePath = IRIDIUM_OUTPUTS_FOLDER + std::string("AfterStackCollapse_") + std::to_string(getStartBBIDX()) + ".iridump";
    std::filesystem::path dir = IRIDIUM_OUTPUTS_FOLDER;
    try
    {
      if (!std::filesystem::exists(dir))
      {
        std::filesystem::create_directories(dir);
      }

      std::ofstream outFile(savePath);
      if (!outFile)
      {
        throw std::runtime_error("Could not open file: " + savePath);
      }
      DBG("IRIDIUM_DUMP_BEFORE_STACK_COLLAPSE");
      targetContainer->prettyPrint(outFile);
      outFile << std::endl;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
      std::cerr << "[IRIDIUM_DUMP_BEFORE_STACK_COLLAPSE] Filesystem error: " << e.what() << '\n';
    }
  }
#endif

#if IRIDIUM_DUMP_FINAL_CFG == 1
  {
    DBG("IRIDIUM_DUMP_FINAL_CFG");
    std::string savePath = IRIDIUM_OUTPUTS_FOLDER + std::string("Final_CFG_BB") + std::to_string(this->getStartBBIDX()) + ".DOT";
    std::filesystem::path dir = IRIDIUM_OUTPUTS_FOLDER;
    try
    {
      if (!std::filesystem::exists(dir))
      {
        std::filesystem::create_directories(dir);
      }
      cfgManager.dumpCFGDOT(savePath);
    }
    catch (const std::filesystem::filesystem_error &e)
    {
      std::cerr << "[IRIDIUM_DUMP_FINAL_CFG] Filesystem error: " << e.what() << '\n';
    }
  }
#endif
  return targetContainer;
}

std::set<IRISEXP> BBContainerView::getAllStackBindings()
{
  std::set<IRISEXP> res;
  for (auto &e : bindingsView.bindings)
  {
    for (auto &b : e.second)
    {
      res.insert(b);
    }
  }
  return res;
}

std::set<IRISEXP> BBContainerView::getUncapturedStackBindings()
{
  static const std::unordered_set<std::string> implicitBindings = {
    // "arguments", 
    // "this.active_func", 
    // "new.target", 
    // "<home_object>",
    // "<var_obj>",
    // "<module_meta>",
    // "<super_ctr>",
    // "<super_obj>",
    // "this"
  };

  std::set<IRISEXP> res;
  for (auto &e : bindingsView.bindings)
  {
    for (auto &b : e.second)
    {
      // if (implicitBindings.find(b->getNAME()) != implicitBindings.end()) {
      //   continue;
      // }
      auto &curr = symbolTable[b];
      if (curr.remoteReads.size() == 0 && curr.remoteWrites.size() == 0)
        res.insert(b);
    }
  }
  return res;
}

std::set<std::shared_ptr<EnvBindingSEXP>> BBContainerView::getCapturedStackBindings()
{
  static const std::unordered_set<std::string> implicitBindings = {
    // "arguments", 
    // "this.active_func", 
    // "new.target", 
    // "<home_object>",
    // "<var_obj>",
    // "<module_meta>",
    // "<super_ctr>",
    // "<super_obj>",
    // "this"
  };

  std::set<std::shared_ptr<EnvBindingSEXP>> res;
  for (auto &e : bindingsView.bindings)
  {
    for (auto &b : e.second)
    {
      // if (implicitBindings.find(b->getNAME()) != implicitBindings.end()) {
      //   res.insert(b);
      //   continue;
      // }
      auto &curr = symbolTable[b];
      if (curr.remoteReads.size() > 0 || curr.remoteWrites.size() > 0)
        res.insert(b);
    }
  }
  return res;
}

bool BBContainerView::hasImplicitBindings()
{
  std::set<std::shared_ptr<EnvBindingSEXP>> res;
  for (auto &e : bindingsView.bindings)
  {
    for (auto &b : e.second)
    {
      if (
          b->getNAME() == "arguments" 
          || b->getNAME() == "<this_func>" 
          || b->getNAME() == "<new_target>" 
          || b->getNAME() == "<home_obj>" 
          || b->getNAME() == "<var_obj>" 
          || b->getNAME() == "<module_meta>" 
          || b->getNAME() == "<super_ctr>" 
          || b->getNAME() == "<super_obj>" 
          || b->getNAME() == "this" 
          || b->getNAME() == "ret"
        )
        return true;
    }
  }
  return false;
}

json BBContainerView::getDebugJSON(const std::string &title)
{
  json data_object = {
      {"title", title},
  };

  data_object["codeText"] = cfgManager.getDebugJSON();
  return data_object;
}