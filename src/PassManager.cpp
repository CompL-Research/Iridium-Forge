#include "Iridium/PassManager.h"

#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/Domains/EffectAtStmt.h"
#include "Iridium/Analysis/Domains/CopyPropInfo.h"
#include "Iridium/Analysis/Domains/SetSafePropKeyAccesses.h"
#include "Iridium/Analysis/DataflowSolver.h"
#include "Iridium/OptimizationPasses/ConstantProp.h"
#include "Iridium/OptimizationPasses/WriteBarrierReduction.h"
#include "Iridium/OptimizationPasses/CopyProp.h"
#include "Iridium/OptimizationPasses/PropEffects.h"
#include "Iridium/OptimizationPasses/DCE.h"
#include "Iridium/OptimizationPasses/DeadBindingRemoval.h"
#include "Iridium/OptimizationPasses/ReduceComputedFieldOps.h"
#include "Iridium/OptimizationPasses/RemoveRedundantPropKeyCast.h"
#include "Iridium/CorePasses/3_filterNops.h"
#include "external/json.hpp"
#include <fstream>

#include "external/Prakriti.hpp"

#include <filesystem>

#include <string>
#include <random>

#include <chrono>
#include <iostream>

#include <iostream>
#include <cstdlib>
#include <string>

void printOptimizationStatus()
{
  struct Flag
  {
    const char *env;  // NO_ flag
    const char *name; // positive feature name
  };

  Flag flags[] = {
      {"NO_CONSTPROP", "CONSTPROP"},
      {"NO_COPYPROP", "COPYPROP"},
      {"NO_WBR", "WBR"},
      {"NO_DCE", "DCE"},
      {"NO_EPROP", "EPROP"},
      {"NO_RKEYCAST", "RKEYCAST"},
      {"NO_REDKEYCAST", "REDKEYCAST"},
      {"NO_DEADBR", "DEADBR"}};

  for (const auto &f : flags)
  {
    bool disabled = (std::getenv(f.env) != nullptr);
    std::cerr << f.name << ": " << (disabled ? "OFF" : "ON") << "\n";
  }
}

#define INLINING_DEPTH 1

class ScopeTimer
{
public:
  explicit ScopeTimer(const std::string &name = "")
      : name_(name),
        start_(std::chrono::high_resolution_clock::now()) {}

  ~ScopeTimer()
  {
    using namespace std::chrono;
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start_).count();

    std::cerr << "[TIMER] " << (name_.empty() ? "Scope" : name_)
              << " took " << duration / 1000.0 << " ms\n";
  }

private:
  std::string name_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

std::string randomString(size_t length)
{
  static const std::string chars =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789";

  thread_local static std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<size_t> dist(0, chars.size() - 1);

  std::string result;
  result.reserve(length);
  for (size_t i = 0; i < length; i++)
  {
    result.push_back(chars[dist(rng)]);
  }
  return result;
}

using json = nlohmann::json;

void PassManager::justAnalysis(std::stringstream &ss, std::set<double> taintedScopes)
{
  size_t envReadRemoteTotal = 0, envReadRemoteSafe = 0;
  size_t envWriteRemoteTotal = 0, envWriteRemoteSafe = 0;

  std::function<void(IRISEXP currSEXP)> collectInfo = [&](IRISEXP currSEXP)
  {
    if (auto read = std::dynamic_pointer_cast<EnvReadSEXP>(currSEXP))
    {
      if (read->hasSAFE() && read->hasFlag("MAP_INF"))
      {
        // std::cout << "SAFEREAD: ";
        // read->prettyPrint(std::cout);
        // std::cout << std::endl;
        auto mapInf = read->getFlagString("MAP_INF");
        if (mapInf.find("js3$") == std::string::npos && mapInf != "NA")
        {
          ss << mapInf;
        }
      }

      if (auto o = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(read->getObj()))
      {
        envReadRemoteTotal++;
        if (read->hasSAFE())
        {
          envReadRemoteSafe++;
        }
      }

      return;
    }
    else if (auto write = std::dynamic_pointer_cast<EnvWriteSEXP>(currSEXP))
    {
      if (write->hasSAFE() && write->hasFlag("MAP_INF"))
      {
        // std::cout << "SAFEWRITE: ";
        // write->prettyPrint(std::cout);
        // std::cout << std::endl;
        auto mapInf = write->getFlagString("MAP_INF");
        if (mapInf.find("js3$") == std::string::npos && mapInf != "NA")
        {
          ss << mapInf;
        }
      }

      if (auto o = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(write->getLValTarget()))
      {
        envWriteRemoteTotal++;
        if (write->hasSAFE())
        {
          envWriteRemoteSafe++;
        }
      }
    }
    for (auto &e : currSEXP->args)
      collectInfo(e);
  };

  for (int i = 0; i < 5; i++)
  {
    for (auto &bbContView : fileView.bbContainerViews)
    {

      // Skip the container if it is reachable from a tainted scope...
      for (auto &ts : taintedScopes)
      {
        if (isScopeReachable(ts, bbContView.getScopeIdx(), iridiumBuildContext))
        {
          bbContView.tainted = true;
          continue;
        }
      }

      // fileView.refreshSymbolTable();
      auto allStackBindings = bbContView.getAllStackBindings();
      auto capturedStackBindings = bbContView.getCapturedStackBindings();
      auto uncapturedStackBindings = bbContView.getUncapturedStackBindings();

      CopyPropInfo::blacklist = capturedStackBindings;
      Liveness::blacklist = capturedStackBindings;
      ConstantsAtStmt::blacklist = capturedStackBindings;
      EffectAtStmt::blacklist = capturedStackBindings;
      SetSafePropKeyAccesses::blacklist = capturedStackBindings;

      {
        WriteBarrierReduction::currFileView = &fileView;
        DataflowSolver<TDZA> tdzaSolver(bbContView.cfgManager, true, [&]()
                                        { return TDZA::bottom(allStackBindings); });
        for (auto &e : tdzaSolver.run(TDZA::boundary(allStackBindings)))
        {
          WriteBarrierReduction::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
      }
    }
  }

  for (auto &bbContView : fileView.bbContainerViews)
  {
    bbContView.cfgManager.traverseCFG(
        [&](Vertex v, std::shared_ptr<BBSEXP> bb)
        {
          for (auto &stmt : bb->args)
          {
            collectInfo(stmt);
          }
        });
  }

  // std::cout << ss.str() << std::endl;
  std::cout << "SAFE: " << (envReadRemoteSafe + envWriteRemoteSafe) << " TOTAL: " << (envReadRemoteTotal + envWriteRemoteTotal) << std::endl;
}

std::unordered_map<IRISEXP, size_t> globalMap;
std::unordered_map<size_t, IRISEXP> inverseMap;
static size_t inc = 0;

size_t getNodeID(IRISEXP sexp) {
  if (globalMap.count(sexp)) return globalMap[sexp];
  inverseMap[inc] = sexp;
  globalMap[sexp] = inc++; 
}

void PassManager::pta(int level, std::set<double> taintedScopes)
{
  // When performing PTA. There are two important levels of granularity.
  // 1. Modelling file level behaviour.
  // 2. Modelling function level behaviour.
  // 
  // - For our analysis we can model them similarly, but keep separate nodes (to prevent duplicate analysis for a pre analyzed module/script).
  // 


  JSGraph::ECMAGraph mainGraph;
  std::cout << "Starting PTA Analysis" << std::endl;
  auto &topLevelContainer = fileView.getTopLevelContainer();

  for (auto sBinding : topLevelContainer.getAllStackBindings()) {
    size_t id = getNodeID(sBinding);
    JSGraph::Bindu<JSGraph::BinduId> b(id, JSGraph::TAG::STACK);

    b.RegisterAction(JSGraph::ActionTag::STACK_set_strong, JSGraph::AC_STACK_set_strong);
    b.RegisterAction(JSGraph::ActionTag::STACK_set_weak, JSGraph::AC_STACK_set_weak);

    if (!mainGraph.hasNode(id)) mainGraph.addNode(b);
  }

  topLevelContainer.cfgManager.traverseCFG(
      [&](Vertex v, std::shared_ptr<BBSEXP> bb)
      {
        for (auto &stmt : bb->args)
        {

          auto currGraph = mainGraph.clone();

          auto iBindingDecl = std::dynamic_pointer_cast<JSImplicitBindingDeclarationSEXP>(stmt);
          if (iBindingDecl)
          {
            std::cout << stmt->tag << " (TODO)" << std::endl;
            continue;
          }

          auto eWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt);

          if (eWrite)
          {
            // 
            // Gen New Object (IRISEXP curr)
            // JSObjectSEXP :=> Ordinary Object
            // 
            // 1.  -> Gen Ordinary Object
            // 2. Stack Object -> 
            // 
            // 
            // LVAL = RVAL 
            // For all LVALS, call the [[set]] method...
            //       LVAL
            //          - EnvBinding             -> Stack Object
            //          - RemoteEnvBinding       -> Resolve until Stack Object (some assertion that this node exists at context entry should be implicitly guaranteed)
            //          - GlobalBinding          -> This binding looking from the global this object
            // 
            //       RVAL
            //          - Ordinary Object
            //          - Stack Object
            //          - JSNUBD                 // (TODO)
            //          - ...Constants as a WIP...
            //
            // a = {}
            {
              auto LVAL = eWrite->getLValTarget();
              
              auto RVAL = eWrite->getRVal();
              if (
                std::dynamic_pointer_cast<EnvBindingSEXP>(LVAL) &&
                std::dynamic_pointer_cast<JSObjectSEXP>(RVAL)
              ) {
                auto lid = getNodeID(LVAL);
                auto rid = getNodeID(RVAL);

                std::cout << "ENV WRITE: LVAL ->" << lid << ", " << " RVAL: " << rid << std::endl;
                auto bin = mainGraph.GetNodeDescriptor(lid);

                assert(bin.HasAction(JSGraph::ActionTag::STACK_set_strong));
                auto actions = bin.GetActions(JSGraph::ActionTag::STACK_set_strong);

                for (auto & a : actions) {
                  currGraph = a(currGraph, []); // ... add args for action closure...
                }
              }
            } 

            
          }
        }
      });
}

void PassManager::optimize(int level, std::set<double> taintedScopes)
{
  if (getenv("PRINT_OPT_STAT"))
    printOptimizationStatus();
#if PASSMGR_DEBUG == 1
  std::filesystem::path out_path = "outputs/passmanager.json";
  json PASSMGR_DEBUGInfo = {
      {"data", json::array()}};
  std::vector<json> PASSMGR_DEBUGVector;
#endif

  // Generate global inlining targets

  // auto &topLevelContainer = fileView.getTopLevelContainer();

  // struct SinLining
  // {
  //   BBContainerView *targetContainer;
  //   std::string guard;
  //   IRISEXP guardStoreLoc; // We will remove this if the sinlining never uses it later
  //   bool used;
  // };

  // std::unordered_map<std::string, SinLining> sinlining;

  // double sinIDX = -1;

  // topLevelContainer.cfgManager.traverseCFG(
  //     [&](Vertex v, std::shared_ptr<BBSEXP> bb)
  //     {
  //       for (auto it = bb->args.begin(); it != bb->args.end(); /* no ++ here */)
  //       {
  //         if (auto funcDecl = std::dynamic_pointer_cast<JSFuncDeclSEXP>(*it))
  //         {
  //           auto store = std::dynamic_pointer_cast<GlobalBindingSEXP>(funcDecl->getLValTarget());
  //           auto poolBinding = std::dynamic_pointer_cast<PoolBindingSEXP>(funcDecl->getRVal());
  //           assert(store && poolBinding);
  //           auto lambda = std::dynamic_pointer_cast<LambdaSEXP>(poolBinding->getLambda());
  //           assert(lambda);

  //           std::string guardName = randomString(5);
  //           IRISEXP guardStoreLoc = std::make_shared<EnvWriteSEXP>(
  //               std::make_shared<GlobalBindingSEXP>(guardName),
  //               std::make_shared<GlobalBindingSEXP>(store->getNAME()),
  //               true, false, true, false);

  //           it = bb->args.insert(std::next(it), guardStoreLoc); // returns iterator to new element
  //           ++it;                                               // advance past inserted element

  //           // std::cout << "adding to sinlining table: " << store->getNAME() << std::endl;

  //           sinlining[store->getNAME()] = {
  //               &fileView.getContainer(lambda->getStartBBIDX()),
  //               guardName,
  //               guardStoreLoc,
  //               false};
  //         }
  //         ++it; // move to next original element
  //       }
  //     });

  // std::set<IRISEXP> alreadyInlined;

  // std::unordered_map<BBContainerView *, std::unordered_map<std::string, int>> inliningMetadata;

  for (int i = 0; i < 5; i++)
  {
    // DBG("Iter: " + std::to_string(i));

    ScopeTimer timer("Iter: " + std::to_string(i));

    // std::cout << "::Inlining metadata::" << std::endl;
    // for (auto &e : inliningMetadata)
    // {
    //   std::cout << "  BB" << e.first->getStartBBIDX() << std::endl;
    //   for (auto &j : e.second)
    //   {
    //     std::cout << "    " << j.first << " : " << j.second << std::endl;
    //   }
    // }

    for (auto &bbContView : fileView.bbContainerViews)
    {

      // Skip the container if it is reachable from a tainted scope...
      for (auto &ts : taintedScopes)
      {
        if (isScopeReachable(ts, bbContView.getScopeIdx(), iridiumBuildContext))
        {
          bbContView.tainted = true;
          continue;
        }
      }

      // fileView.refreshSymbolTable();
      auto allStackBindings = bbContView.getAllStackBindings();
      auto capturedStackBindings = bbContView.getCapturedStackBindings();
      auto uncapturedStackBindings = bbContView.getUncapturedStackBindings();

      CopyPropInfo::blacklist = capturedStackBindings;
      Liveness::blacklist = capturedStackBindings;
      ConstantsAtStmt::blacklist = capturedStackBindings;
      EffectAtStmt::blacklist = capturedStackBindings;
      SetSafePropKeyAccesses::blacklist = capturedStackBindings;

      std::string passBasename = "Iter" + std::to_string((int)i) + "_BB" + std::to_string((int)bbContView.getStartBBIDX());

      // // SinLining
      // {
      //   // ScopeTimer timer("SINLINING");
      //   auto &currCFGManager = bbContView.cfgManager;
      //   auto &currCFG = currCFGManager.cfg;

      //   std::vector<std::shared_ptr<BBSEXP>> noRetInliningList;

      //   std::vector<std::shared_ptr<BBSEXP>> retInliningList;

      //   // Isolate inlining blocks
      //   auto worklist = currCFGManager.getVertices();
      //   while (worklist.size() > 0)
      //   {
      //     Vertex v = worklist.back();
      //     worklist.pop_back();

      //     auto bb = currCFG[v];
      //     bool split = false;

      //     for (int i = 0; i < bb->args.size(); i++)
      //     {
      //       auto xx = bb->args[i];
      //       if (auto stackReject = std::dynamic_pointer_cast<StackRejectSEXP>(xx))
      //       {
      //         if (auto callSite = std::dynamic_pointer_cast<CallSiteSEXP>(stackReject->args[0]))
      //         {
      //           if (alreadyInlined.count(callSite) > 0)
      //             continue;
      //           if (
      //               callSite->hasCCall() || callSite->hasConstructorCall() || callSite->hasPrivateCall() || callSite->hasImport() || callSite->hasSuper() || callSite->hasV8Intrinsic())

      //           {
      //           }
      //           else
      //           {
      //             // CALLSITE()
      //             auto callee = callSite->args[0];
      //             if (auto gg = std::dynamic_pointer_cast<EnvReadSEXP>(callee))
      //             {
      //               if (auto globalCallee = std::dynamic_pointer_cast<GlobalBindingSEXP>(gg->getObj()))
      //               {
      //                 auto targetName = globalCallee->getNAME();
      //                 if (sinlining.count(targetName))
      //                 {
      //                   // do inlining
      //                   // set LVAL = NULL
      //                   auto sinfo = sinlining[targetName];
      //                   if (sinfo.targetContainer == &bbContView)
      //                     continue;
      //                   if (sinfo.targetContainer->getCapturedStackBindings().size() == 0)
      //                   {
      //                     if (!sinfo.targetContainer->hasImplicitBindings())
      //                     {

      //                       if (inliningMetadata[&bbContView][targetName] < INLINING_DEPTH)
      //                       {
      //                         inliningMetadata[&bbContView][targetName]++;
      //                         // Dont try to inline things inside the fallthrough block later
      //                         alreadyInlined.insert(callSite);

      //                         // Split Isolate BB where inlining must be performed
      //                         // this ensures that function being inlined is the last statement in the basic block.
      //                         auto continuationIDX = sinIDX--;
      //                         auto continuationBB = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), continuationIDX, bb->getScopeIDX());
      //                         continuationBB->args.assign(bb->args.begin() + i + 1, bb->args.end());
      //                         bb->args.erase(bb->args.begin() + i + 1, bb->args.end());

      //                         bb->args.push_back(std::make_shared<GotoSEXP>(continuationIDX));

      //                         auto continuationVertex = currCFGManager.addNode(continuationBB);
      //                         currCFGManager.transferSuccessors(v, continuationVertex);
      //                         currCFGManager.connect(v, continuationVertex, {EdgeKind::Normal, stackReject});

      //                         // Add the current BB to the inlining list
      //                         noRetInliningList.push_back(bb);

      //                         // Add the newly created BB to the worklist
      //                         worklist.push_back(continuationVertex);
      //                       }
      //                       else
      //                       {
      //                         // std::cout << "Stopping inling at 5" << std::endl;
      //                       }
      //                     }
      //                     else
      //                     {
      //                       // std::cout << "SinLining target has implicit bindings" << std::endl;
      //                     }
      //                   }
      //                   else
      //                   {
      //                     // std::cout << "SinLining target has captured bindings" << std::endl;
      //                   }
      //                 }
      //               }
      //             }
      //           }
      //         }
      //       }
      //     }
      //   }

      //   for (auto &bb : noRetInliningList)
      //   {
      //     // std::cout << "Bindings Before Inlining" << std::endl;
      //     // bbContView.bindingsView.dump(std::cout);

      //     // bbContView.cfgManager.dumpCFGDOT("outputs/BEFORE_INLINING_" + std::to_string(sinIDX));

      //     auto stackRejectStmt = std::dynamic_pointer_cast<StackRejectSEXP>(bb->args.at(bb->args.size() - 2));
      //     assert(stackRejectStmt);
      //     auto callSite = std::dynamic_pointer_cast<CallSiteSEXP>(stackRejectStmt->args[0]);
      //     assert(callSite);
      //     auto calleeEnvRead = std::dynamic_pointer_cast<EnvReadSEXP>(callSite->args[0]);
      //     assert(calleeEnvRead);
      //     auto callee = std::dynamic_pointer_cast<GlobalBindingSEXP>(calleeEnvRead->args[0]);
      //     assert(callee);
      //     assert(currCFGManager.bbIdxToVertex.count(bb->getIDX()) > 0);
      //     assert(sinlining.count(callee->getNAME()) > 0);

      //     auto sinfo = sinlining[callee->getNAME()];

      //     // Clone the container at this stage, it cannot be a broken CFG at this stage
      //     auto clonedContainer = sinfo.targetContainer->clone(sinIDX);

      //     // bb --> continuation
      //     auto currVertex = currCFGManager.bbIdxToVertex[bb->getIDX()];
      //     auto continuations = currCFGManager.successors(currVertex);
      //     assert(continuations.size() == 1);
      //     auto continuationVertex = continuations[0];
      //     auto continuationBB = currCFGManager.cfg[continuationVertex];
      //     auto continuationIDX = continuationBB->getIDX();

      //     // Remove the last two statements from the BB
      //     bb->args.resize(bb->args.size() - 2);

      //     // Clear outgoing edges from the bb --> continuation => bb -->
      //     clear_out_edges(currVertex, currCFGManager.cfg);

      //     // CurrBB is broken now, add SIN TRUE and SIN FALSE Blocks
      //     // SIN TRUE BLOCK
      //     auto sinTrueIDX = sinIDX--;
      //     auto sinTrueBlock = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), sinTrueIDX, bb->getScopeIDX());
      //     {
      //       // Do argument passing...
      //       auto argBindingsInInlinedCode = clonedContainer.bindingsView.args;
      //       std::vector<IRISEXP> callSiteArgs(callSite->args.begin() + 1, callSite->args.end());

      //       std::sort(argBindingsInInlinedCode.begin(), argBindingsInInlinedCode.end(),
      //                 [](const IRISEXP &a, const IRISEXP &b)
      //                 {
      //                   auto a1 = std::dynamic_pointer_cast<EnvBindingSEXP>(a);
      //                   auto b1 = std::dynamic_pointer_cast<EnvBindingSEXP>(b);
      //                   assert(a1);
      //                   assert(b1);
      //                   assert(a1->hasJSARG() || a1->hasJSRESTARG());
      //                   assert(b1->hasJSARG() || b1->hasJSRESTARG());
      //                   return a1->getREFIDX() < b1->getREFIDX();
      //                 });
      //       for (size_t formalIdx = 0; formalIdx < argBindingsInInlinedCode.size(); formalIdx++)
      //       {
      //         IRISEXP LVAL = argBindingsInInlinedCode[formalIdx];
      //         IRISEXP RVAL;
      //         if (formalIdx >= callSiteArgs.size())
      //         {
      //           RVAL = std::make_shared<EnvReadSEXP>(
      //               std::make_shared<GlobalBindingSEXP>("undefined"),
      //               false);
      //         }
      //         else
      //         {
      //           RVAL = callSiteArgs[formalIdx];
      //         }
      //         // IRISEXP LValTarget, IRISEXP RVal, bool SLOPPY, bool THROWERR, bool SAFE, bool THISINIT
      //         sinTrueBlock->args.push_back(
      //             std::make_shared<EnvWriteSEXP>(LVAL, RVAL, false, false, true, false));
      //       }
      //     }
      //     auto sinTrueVertex = currCFGManager.addNode(sinTrueBlock);

      //     // SIN FALSE BLOCK
      //     auto sinFalseIDX = sinIDX--;
      //     auto sinFalseBlock = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), sinFalseIDX, bb->getScopeIDX());
      //     {
      //       // bool CCall, bool ConstructorCall, bool PrivateCall, bool Import, bool Super, bool V8Intrinsic, bool TAILCALL, double JSDirectEval
      //       auto cs = std::make_shared<CallSiteSEXP>(false, false, false, false, false, false, false, false);
      //       cs->unsetJSDirectEval();

      //       cs->args.push_back(
      //           std::make_shared<FieldReadSEXP>(
      //               std::make_shared<EnvReadSEXP>(
      //                   std::make_shared<GlobalBindingSEXP>("console"),
      //                   false),
      //               std::make_shared<StringSEXP>("log")));

      //       cs->args.push_back(
      //           std::make_shared<StringSEXP>("SIN FALSE"));

      //       auto o = std::make_shared<StackRejectSEXP>(1);
      //       o->args.push_back(cs);
      //       sinFalseBlock->args.push_back(o);

      //       // Add console.log, SIN FALSE
      //       // Fallback
      //       sinFalseBlock->args.push_back(stackRejectStmt);
      //       sinFalseBlock->args.push_back(std::make_shared<GotoSEXP>(continuationIDX));
      //     }
      //     auto sinFlaseVertex = currCFGManager.addNode(sinFalseBlock);

      //     // IRISEXP LBinop, IRISEXP RBinop, std::string OP
      //     auto test = std::make_shared<BinopSEXP>(
      //         std::make_shared<EnvReadSEXP>(
      //             std::make_shared<GlobalBindingSEXP>(callee->getNAME()),
      //             false),
      //         std::make_shared<EnvReadSEXP>(
      //             std::make_shared<GlobalBindingSEXP>(sinfo.guard),
      //             false),
      //         "===");
      //     // IRISEXP Test, bool NOT, double TRUE, double FALSE
      //     bb->args.push_back(std::make_shared<IfElseJumpSEXP>(test, false, sinTrueIDX, sinFalseIDX));

      //     currCFGManager.connect(bb, sinTrueBlock, {EdgeKind::Normal, stackRejectStmt});
      //     currCFGManager.connect(bb, sinFalseBlock, {EdgeKind::Normal, stackRejectStmt});
      //     currCFGManager.connect(sinFalseBlock, continuationBB, {EdgeKind::Normal, stackRejectStmt});

      //     // Demote arg bindings to the stack frame
      //     clonedContainer.bindingsView.demoteArgumentsToRoot();

      //     // Merge Stack Frames... remember to copy over remote bindings as-well
      //     bbContView.bindingsView.mergeBindingsTree(bb->getScopeIDX(), clonedContainer.bindingsView);

      //     //                                ------------------------
      //     // Inline, SINTRUEBB       ----> |  Inlined Code Entry BB |
      //     //         CONTINUATIONBB  <---- |  [sinks i.e. return]   |
      //     //                                ------------------------
      //     //
      //     currCFGManager.doInlining(clonedContainer.cfgManager, sinTrueBlock, continuationBB);

      //     fileView.refreshSymbolTable();
      //     allStackBindings = bbContView.getAllStackBindings();
      //     capturedStackBindings = bbContView.getCapturedStackBindings();
      //     uncapturedStackBindings = bbContView.getUncapturedStackBindings();

      //     CopyPropInfo::blacklist = capturedStackBindings;
      //     Liveness::blacklist = capturedStackBindings;
      //     ConstantsAtStmt::blacklist = capturedStackBindings;
      //     EffectAtStmt::blacklist = capturedStackBindings;
      //     SetSafePropKeyAccesses::blacklist = capturedStackBindings;
      //   }
      // }

#if PASSMGR_DEBUG == 1
      PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_START"));
#endif

      if (!getenv("NO_CONSTPROP"))
      {
        // ScopeTimer timer("ConstantProp");
        DataflowSolver<ConstantsAtStmt> constantsAtStmtSolver(bbContView.cfgManager, true, [&]()
                                                              { return ConstantsAtStmt::bottom(); });
        for (auto &e : constantsAtStmtSolver.run(ConstantsAtStmt::boundary()))
        {
          ConstantProp::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
#if PASSMGR_DEBUG == 1
        // DBG("End ConstantProp");
        PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_1_CONSTANT_PROP"));
#endif
      }

      if (!getenv("NO_COPYPROP"))
      {
        // ScopeTimer timer("CopyProp");
        DataflowSolver<CopyPropInfo> copyPropInfoSolver(bbContView.cfgManager, true, [&]()
                                                        { return CopyPropInfo(); });
        for (auto &e : copyPropInfoSolver.run(CopyPropInfo()))
        {
          auto currBB = bbContView.cfgManager.cfg[e.first];
          CopyProp::Transform(currBB, e.second);
        }
#if PASSMGR_DEBUG == 1
        // DBG("End CopyProp");
        PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_2_COPY_PROP"));
#endif
      }

      if (!getenv("NO_WBR"))
      {
        // ScopeTimer timer("WriteBarrierReduction");
        WriteBarrierReduction::currFileView = &fileView;
        DataflowSolver<TDZA> tdzaSolver(bbContView.cfgManager, true, [&]()
                                        { return TDZA::bottom(allStackBindings); });
        for (auto &e : tdzaSolver.run(TDZA::boundary(allStackBindings)))
        {
          WriteBarrierReduction::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
#if PASSMGR_DEBUG == 1
        // DBG("End WriteBarrierReduction");
        PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_3_WBR"));
#endif
      }

      if (!getenv("NO_DCE"))
      {
        // ScopeTimer timer("DCE");
        DataflowSolver<Liveness> livenessSolver(bbContView.cfgManager, false, [&]()
                                                { return Liveness::bottom(); });
        for (auto &e : livenessSolver.run(Liveness::boundary(capturedStackBindings)))
        {
          auto currBB = bbContView.cfgManager.cfg[e.first];
          DCE::Transform(currBB, e.second);
          filterNOPs(currBB);
        }
#if PASSMGR_DEBUG == 1
        // DBG("End DCE");
        PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_4_DCE"));
#endif
      }

      if (!getenv("NO_EPROP"))
      {
        // ScopeTimer timer("EffectProp");
        DataflowSolver<Liveness> livenessSolver(bbContView.cfgManager, false, [&]()
                                                { return Liveness::bottom(); });
        DataflowSolver<EffectAtStmt> effectSolver(bbContView.cfgManager, true, [&]()
                                                  { return EffectAtStmt(); });
        std::unordered_map<Vertex, Liveness> livenessInfo;
        // std::unordered_map<Vertex, EffectAtStmt&> effectInfo;

        std::set<IRISEXP> killset;

        for (auto &e : livenessSolver.run(Liveness::boundary(capturedStackBindings)))
        {
          livenessInfo[e.first] = e.second;
        }

        // TODO: Assert livenessInfo.size == effectInfo.size...
        for (auto &e : effectSolver.run(EffectAtStmt()))
        {
          auto currBB = bbContView.cfgManager.cfg[e.first];
          PropEffects::Transform(currBB, killset, livenessInfo[e.first], e.second);
        }

        // Kill all moved statements...
        auto killer = [&](IRISEXP curr)
        {
          for (auto it = curr->args.begin(); it != curr->args.end();)
          {
            if (killset.count(*it))
            {
              it = curr->args.erase(it); // erase returns next valid iterator
            }
            else
            {
              ++it; // only increment if not erased
            }
          }
        };

        for (auto v : boost::make_iterator_range(boost::vertices(bbContView.cfgManager.cfg)))
        {
          auto currBB = bbContView.cfgManager.cfg[v];
          killer(currBB);
        }
#if PASSMGR_DEBUG == 1
        // DBG("End Effect Prop");
        PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_5_EFFECT_PROP"));
#endif
      }

      if (!getenv("NO_RKEYCAST"))
      {
        // ScopeTimer timer("RemoveRedundantPropKeyCast");
        // std::cout << "Starting SetSafePropKeyAccesses" << std::endl;
        DataflowSolver<SetSafePropKeyAccesses> setSafePropKeyAccesses(bbContView.cfgManager, true, [&]()
                                                                      { return SetSafePropKeyAccesses::bottom(uncapturedStackBindings); });

        for (auto &e : setSafePropKeyAccesses.run(SetSafePropKeyAccesses::boundary(uncapturedStackBindings)))
        {
          RemoveRedundantPropKeyCast::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
#if PASSMGR_DEBUG == 1
        // DBG("End SetSafePropKeyAccesses");
        PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_5_SAFE_PROP_KEY_ACCESS"));
#endif
      }

      if (!getenv("NO_REDKEYCAST"))
      {
        // ScopeTimer timer("reduceComputedFieldOps");
        reduceComputedFieldOps(bbContView);
      }
    }
  }
  if (!getenv("NO_DEADBR"))
  {
    // ScopeTimer timer("doDeadBindingRemoval");
    doDeadBindingRemoval(fileView);
  }

  // Mark Tail Calls
  for (auto &bbContView : fileView.bbContainerViews)
  {
    bbContView.cfgManager.traverseCFG(
        [&](Vertex v, std::shared_ptr<BBSEXP> bb)
        {
          for (auto &stmt : bb->args)
          {
            if (auto retStmt = std::dynamic_pointer_cast<ReturnSEXP>(stmt))
            {
              if (auto tailCall = std::dynamic_pointer_cast<CallSiteSEXP>(retStmt->getObj()))
              {
                tailCall->setTAILCALL();
              }
            }
          }
        });
  }

#if PASSMGR_DEBUG == 1
  std::filesystem::create_directories(out_path.parent_path());

  for (const auto &item : PASSMGR_DEBUGVector)
  {
    PASSMGR_DEBUGInfo["data"].push_back(item);
  }

  std::ofstream o(out_path);
  o << PASSMGR_DEBUGInfo.dump(2) << std::endl;
  o.close();
#endif
}

std::shared_ptr<FileSEXP> PassManager::checkout()
{
  return fileView.checkout();
}

// for (auto &bb : retInliningList)
// {
//   // std::cout << "Bindings Before Inlining" << std::endl;
//   // bbContView.bindingsView.dump(std::cout);

//   // bbContView.cfgManager.dumpCFGDOT("outputs/BEFORE_INLINING_" + std::to_string(sinIDX));

//   auto envWriteStmt = std::dynamic_pointer_cast<EnvWriteSEXP>(bb->args.at(bb->args.size() - 2));
//   assert(envWriteStmt);
//   auto callSite = std::dynamic_pointer_cast<CallSiteSEXP>(envWriteStmt->getRVal());
//   assert(callSite);
//   auto calleeEnvRead = std::dynamic_pointer_cast<EnvReadSEXP>(callSite->args[0]);
//   assert(calleeEnvRead);
//   auto callee = std::dynamic_pointer_cast<GlobalBindingSEXP>(calleeEnvRead->args[0]);
//   assert(callee);
//   assert(currCFGManager.bbIdxToVertex.count(bb->getIDX()) > 0);
//   assert(sinlining.count(callee->getNAME()) > 0);

//   auto sinfo = sinlining[callee->getNAME()];

//   // Clone the container at this stage, it cannot be a broken CFG at this stage
//   auto clonedContainer = sinfo.targetContainer->clone(sinIDX);

//   // bb --> continuation
//   auto currVertex = currCFGManager.bbIdxToVertex[bb->getIDX()];
//   auto continuations = currCFGManager.successors(currVertex);
//   assert(continuations.size() == 1);
//   auto continuationVertex = continuations[0];
//   auto continuationBB = currCFGManager.cfg[continuationVertex];
//   auto continuationIDX = continuationBB->getIDX();

//   // Remove the last two statements from the BB
//   bb->args.resize(bb->args.size() - 2);

//   // Clear outgoing edges from the bb --> continuation => bb -->
//   clear_out_edges(currVertex, currCFGManager.cfg);

//   // CurrBB is broken now, add SIN TRUE and SIN FALSE Blocks
//   // SIN TRUE BLOCK
//   auto sinTrueIDX = sinIDX--;
//   auto sinTrueBlock = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), sinTrueIDX, bb->getScopeIDX());
//   {
//     // Do argument passing...
//     auto argBindingsInInlinedCode = clonedContainer.bindingsView.args;
//     std::vector<IRISEXP> callSiteArgs(callSite->args.begin() + 1, callSite->args.end());

//     std::sort(argBindingsInInlinedCode.begin(), argBindingsInInlinedCode.end(),
//               [](const IRISEXP &a, const IRISEXP &b)
//               {
//                 auto a1 = std::dynamic_pointer_cast<EnvBindingSEXP>(a);
//                 auto b1 = std::dynamic_pointer_cast<EnvBindingSEXP>(b);
//                 assert(a1);
//                 assert(b1);
//                 assert(a1->hasJSARG() || a1->hasJSRESTARG());
//                 assert(b1->hasJSARG() || b1->hasJSRESTARG());
//                 return a1->getREFIDX() < b1->getREFIDX();
//               });
//     for (size_t formalIdx = 0; formalIdx < argBindingsInInlinedCode.size(); formalIdx++)
//     {
//       IRISEXP LVAL = argBindingsInInlinedCode[formalIdx];
//       IRISEXP RVAL;
//       if (formalIdx >= callSiteArgs.size())
//       {
//         RVAL = std::make_shared<EnvReadSEXP>(
//             std::make_shared<GlobalBindingSEXP>("undefined"),
//             false);
//       }
//       else
//       {
//         RVAL = callSiteArgs[formalIdx];
//       }
//       // IRISEXP LValTarget, IRISEXP RVal, bool SLOPPY, bool THROWERR, bool SAFE, bool THISINIT
//       sinTrueBlock->args.push_back(
//           std::make_shared<EnvWriteSEXP>(LVAL, RVAL, false, false, true, false));
//     }
//   }
//   auto sinTrueVertex = currCFGManager.addNode(sinTrueBlock);

//   // SIN FALSE BLOCK
//   auto sinFalseIDX = sinIDX--;
//   auto sinFalseBlock = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), sinFalseIDX, bb->getScopeIDX());
//   {
//     // bool CCall, bool ConstructorCall, bool PrivateCall, bool Import, bool Super, bool V8Intrinsic, bool TAILCALL, double JSDirectEval
//     auto cs = std::make_shared<CallSiteSEXP>(false, false, false, false, false, false, false, false);
//     cs->unsetJSDirectEval();

//     cs->args.push_back(
//         std::make_shared<FieldReadSEXP>(
//             std::make_shared<EnvReadSEXP>(
//                 std::make_shared<GlobalBindingSEXP>("console"),
//                 false),
//             std::make_shared<StringSEXP>("log")));

//     cs->args.push_back(
//         std::make_shared<StringSEXP>("SIN FALSE"));

//     auto o = std::make_shared<StackRejectSEXP>(1);
//     o->args.push_back(cs);
//     sinFalseBlock->args.push_back(o);

//     // Add console.log, SIN FALSE
//     // Fallback
//     sinFalseBlock->args.push_back(envWriteStmt);
//     sinFalseBlock->args.push_back(std::make_shared<GotoSEXP>(continuationIDX));
//   }
//   auto sinFlaseVertex = currCFGManager.addNode(sinFalseBlock);

//   // IRISEXP LBinop, IRISEXP RBinop, std::string OP
//   auto test = std::make_shared<BinopSEXP>(
//       std::make_shared<EnvReadSEXP>(
//           std::make_shared<GlobalBindingSEXP>(callee->getNAME()),
//           false),
//       std::make_shared<EnvReadSEXP>(
//           std::make_shared<GlobalBindingSEXP>(sinfo.guard),
//           false),
//       "===");
//   // IRISEXP Test, bool NOT, double TRUE, double FALSE
//   bb->args.push_back(std::make_shared<IfElseJumpSEXP>(test, false, sinTrueIDX, sinFalseIDX));

//   currCFGManager.connect(bb, sinTrueBlock, {EdgeKind::Normal, envWriteStmt});
//   currCFGManager.connect(bb, sinFalseBlock, {EdgeKind::Normal, envWriteStmt});
//   currCFGManager.connect(sinFalseBlock, continuationBB, {EdgeKind::Normal, envWriteStmt});

//   // Demote arg bindings to the stack frame
//   clonedContainer.bindingsView.demoteArgumentsToRoot();

//   // Merge Stack Frames... remember to copy over remote bindings as-well
//   bbContView.bindingsView.mergeBindingsTree(bb->getScopeIDX(), clonedContainer.bindingsView);

//   //                                ------------------------
//   // Inline, SINTRUEBB       ----> |  Inlined Code Entry BB |
//   //         CONTINUATIONBB  <---- |  [sinks i.e. return]   |
//   //                                ------------------------
//   //
//   currCFGManager.doInlining(clonedContainer.cfgManager, sinTrueBlock, continuationBB, envWriteStmt);

//   fileView.refreshSymbolTable();
//   allStackBindings = bbContView.getAllStackBindings();
//   capturedStackBindings = bbContView.getCapturedStackBindings();
//   uncapturedStackBindings = bbContView.getUncapturedStackBindings();

//   CopyPropInfo::blacklist = capturedStackBindings;
//   Liveness::blacklist = capturedStackBindings;
//   ConstantsAtStmt::blacklist = capturedStackBindings;
//   EffectAtStmt::blacklist = capturedStackBindings;
//   SetSafePropKeyAccesses::blacklist = capturedStackBindings;
// }

// if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(xx))
// {
//   if (auto callSite = std::dynamic_pointer_cast<CallSiteSEXP>(envWrite->getRVal()))
//   {
//     if (alreadyInlined.count(callSite) > 0)
//       continue;
//     if (
//         callSite->hasCCall() || callSite->hasConstructorCall() || callSite->hasPrivateCall() || callSite->hasImport() || callSite->hasSuper() || callSite->hasV8Intrinsic())
//     {
//     }
//     else
//     {
//       // CALLSITE()
//       auto callee = callSite->args[0];
//       if (auto gg = std::dynamic_pointer_cast<EnvReadSEXP>(callee))
//       {
//         if (auto globalCallee = std::dynamic_pointer_cast<GlobalBindingSEXP>(gg->getObj()))
//         {
//           auto targetName = globalCallee->getNAME();
//           if (sinlining.count(targetName))
//           {
//             // do inlining
//             // set LVAL = NULL
//             auto sinfo = sinlining[targetName];
//             if (sinfo.targetContainer == &bbContView) continue;
//             if (sinfo.targetContainer->getCapturedStackBindings().size() == 0)
//             {
//               if (!sinfo.targetContainer->hasImplicitBindings())
//               {

//                 if (inliningMetadata[&bbContView][targetName] < INLINING_DEPTH)
//                 {
//                   inliningMetadata[&bbContView][targetName]++;
//                   // Dont try to inline things inside the fallthrough block later
//                   alreadyInlined.insert(callSite);

//                   // Split Isolate BB where inlining must be performed
//                   // this ensures that function being inlined is the last statement in the basic block.
//                   auto continuationIDX = sinIDX--;
//                   auto continuationBB = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), continuationIDX, bb->getScopeIDX());
//                   continuationBB->args.assign(bb->args.begin() + i + 1, bb->args.end());
//                   bb->args.erase(bb->args.begin() + i + 1, bb->args.end());

//                   bb->args.push_back(std::make_shared<GotoSEXP>(continuationIDX));

//                   auto continuationVertex = currCFGManager.addNode(continuationBB);
//                   currCFGManager.transferSuccessors(v, continuationVertex);
//                   currCFGManager.connect(v, continuationVertex, {EdgeKind::Normal, envWrite});

//                   // Add the current BB to the inlining list
//                   retInliningList.push_back(bb);

//                   // Add the newly created BB to the worklist
//                   worklist.push_back(continuationVertex);
//                 }
//                 else
//                 {
//                   // std::cout << "Stopping inling at 5" << std::endl;
//                 }
//               }
//               else
//               {
//                 // std::cout << "SinLining target has implicit bindings" << std::endl;
//               }
//             }
//             else
//             {
//               // std::cout << "SinLining target has captured bindings" << std::endl;
//             }
//           }
//         }
//       }
//     }
//   }
// }