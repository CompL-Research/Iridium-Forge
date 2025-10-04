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

#include <filesystem>

#include <string>
#include <random>

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

void PassManager::optimize(int level)
{
#if PASSMGR_DEBUG == 1
  std::filesystem::path out_path = "outputs/passmanager.json";
  json PASSMGR_DEBUGInfo = {
      {"data", json::array()}};
  std::vector<json> PASSMGR_DEBUGVector;
#endif

  // Generate global inlining targets

  auto &topLevelContainer = fileView.getTopLevelContainer();

  struct SinLining
  {
    BBContainerView *targetContainer;
    std::string guard;
    IRISEXP guardStoreLoc; // We will remove this if the sinlining never uses it later
    bool used;
  };

  std::unordered_map<std::string, SinLining> sinlining;

  double sinIDX = -1;

  topLevelContainer.cfgManager.traverseCFG(
      [&](Vertex v, std::shared_ptr<BBSEXP> bb)
      {
        for (auto it = bb->args.begin(); it != bb->args.end(); /* no ++ here */)
        {
          if (auto funcDecl = std::dynamic_pointer_cast<JSFuncDeclSEXP>(*it))
          {
            auto store = std::dynamic_pointer_cast<GlobalBindingSEXP>(funcDecl->getLValTarget());
            auto poolBinding = std::dynamic_pointer_cast<PoolBindingSEXP>(funcDecl->getRVal());
            assert(store && poolBinding);
            auto lambda = std::dynamic_pointer_cast<LambdaSEXP>(poolBinding->getLambda());
            assert(lambda);

            std::string guardName = randomString(5);
            IRISEXP guardStoreLoc = std::make_shared<EnvWriteSEXP>(
                std::make_shared<GlobalBindingSEXP>(guardName),
                std::make_shared<GlobalBindingSEXP>(store->getNAME()),
                true, false, true, false);

            it = bb->args.insert(std::next(it), guardStoreLoc); // returns iterator to new element
            ++it;                                               // advance past inserted element

            std::cout << "adding to sinlining table: " << store->getNAME() << std::endl;

            sinlining[store->getNAME()] = {
                &fileView.getContainer(lambda->getStartBBIDX()),
                guardName,
                guardStoreLoc,
                false};
          }
          ++it; // move to next original element
        }
      });
  
  std::set<IRISEXP> alreadyInlined;

  for (int i = 0; i < 5; i++)
  {
    DBG("Iter: " + std::to_string(i));
    for (auto &bbContView : fileView.bbContainerViews)
    {
      fileView.refreshSymbolTable();
      auto allStackBindings = bbContView.getAllStackBindings();
      auto capturedStackBindings = bbContView.getCapturedStackBindings();
      auto uncapturedStackBindings = bbContView.getUncapturedStackBindings();

      CopyPropInfo::blacklist = capturedStackBindings;
      Liveness::blacklist = capturedStackBindings;
      ConstantsAtStmt::blacklist = capturedStackBindings;
      EffectAtStmt::blacklist = capturedStackBindings;
      SetSafePropKeyAccesses::blacklist = capturedStackBindings;

      std::string passBasename = "Iter" + std::to_string((int)i) + "_BB" + std::to_string((int)bbContView.getStartBBIDX());

      // SinLining
      {
        auto &currCFGManager = bbContView.cfgManager;
        auto &currCFG = currCFGManager.cfg;
        auto worklist = currCFGManager.getVertices();

        while (worklist.size() > 0)
        {
          Vertex v = worklist.back();
          worklist.pop_back();

          auto bb = currCFG[v];
          bool split = false;

          for (int i = 0; i < bb->args.size(); i++)
          {
            if (auto stackReject = std::dynamic_pointer_cast<StackRejectSEXP>(bb->args[i]))
            {
              if (auto callSite = std::dynamic_pointer_cast<CallSiteSEXP>(stackReject->args[0]))
              {
                if (alreadyInlined.count(callSite) > 0) continue;
                if (
                    callSite->hasCCall() || callSite->hasConstructorCall() || callSite->hasPrivateCall() || callSite->hasImport() || callSite->hasSuper() || callSite->hasV8Intrinsic())
                {
                }
                else
                {
                  // do inlining
                  // CALLSITE()
                  auto callee = callSite->args[0];
                  if (auto gg = std::dynamic_pointer_cast<EnvReadSEXP>(callee))
                  {
                    if (auto globalCallee = std::dynamic_pointer_cast<GlobalBindingSEXP>(gg->getObj()))
                    {
                      auto targetName = globalCallee->getNAME();
                      std::cout << "Found possible sinlining target: " << targetName << std::endl;
                      if (sinlining.count(targetName))
                      {
                        // do inlining
                        // set LVAL = NULL
                        std::cout << "Found SinLining target" << std::endl;
                        auto sinfo = sinlining[targetName];
                        if (sinfo.targetContainer->getCapturedStackBindings().size() == 0)
                        {
                          if (!sinfo.targetContainer->hasImplicitBindings())
                          {
                            std::cout << "Ready for sinlining" << std::endl;
                            alreadyInlined.insert(callSite);
                            // Split BB and add a new one to the vertices list

                            // bool TopLevel, bool ClosureBoundary, bool Lexical, double IDX, double ScopeIDX
                            auto continuationIDX = sinIDX--;
                            auto continuationBB = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), continuationIDX, bb->getScopeIDX());
                            continuationBB->args.assign(bb->args.begin() + i + 1, bb->args.end());

                            bb->args.erase(bb->args.begin() + i, bb->args.end()); // Erase the current call instruction aswell

                            auto continuationVertex = currCFGManager.addNode(continuationBB);

                            currCFGManager.transferSuccessors(v, continuationVertex);

                            // SIN TRUE BLOCK
                            auto sinTrueIDX = sinIDX--;
                            auto sinTrueBlock = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), sinTrueIDX, bb->getScopeIDX());
                            {

                              // // bool CCall, bool ConstructorCall, bool PrivateCall, bool Import, bool Super, bool V8Intrinsic, bool TAILCALL, double JSDirectEval
                              // auto cs = std::make_shared<CallSiteSEXP>(false, false, false, false, false, false, false, false);
                              // cs->unsetJSDirectEval();

                              // cs->args.push_back(
                              //     std::make_shared<FieldReadSEXP>(
                              //         std::make_shared<EnvReadSEXP>(
                              //             std::make_shared<GlobalBindingSEXP>("console"),
                              //             false),
                              //         std::make_shared<StringSEXP>("log")));

                              // cs->args.push_back(
                              //     std::make_shared<StringSEXP>("SIN TRUE"));

                              // auto o = std::make_shared<StackRejectSEXP>(1);
                              // o->args.push_back(cs);
                              // sinTrueBlock->args.push_back(o);

                              // Add console.log, SIN TRUE
                              sinTrueBlock->args.push_back(stackReject);
                              sinTrueBlock->args.push_back(std::make_shared<GotoSEXP>(continuationIDX)); // TODO, goto to inlined code here
                            }
                            auto sinTrueVertex = currCFGManager.addNode(sinTrueBlock);

                            // SIN FALSE BLOCK
                            auto sinFalseIDX = sinIDX--;
                            auto sinFalseBlock = std::make_shared<BBSEXP>(bb->hasTopLevel(), bb->hasClosureBoundary(), bb->hasLexical(), sinFalseIDX, bb->getScopeIDX());
                            {

                              // bool CCall, bool ConstructorCall, bool PrivateCall, bool Import, bool Super, bool V8Intrinsic, bool TAILCALL, double JSDirectEval
                              auto cs = std::make_shared<CallSiteSEXP>(false, false, false, false, false, false, false, false);
                              cs->unsetJSDirectEval();

                              cs->args.push_back(
                                  std::make_shared<FieldReadSEXP>(
                                      std::make_shared<EnvReadSEXP>(
                                          std::make_shared<GlobalBindingSEXP>("console"),
                                          false),
                                      std::make_shared<StringSEXP>("log")));

                              cs->args.push_back(
                                  std::make_shared<StringSEXP>("SIN FALSE"));

                              auto o = std::make_shared<StackRejectSEXP>(1);
                              o->args.push_back(cs);
                              sinFalseBlock->args.push_back(o);

                              // Add console.log, SIN FALSE
                              // Fallback
                              sinFalseBlock->args.push_back(stackReject);
                              sinFalseBlock->args.push_back(std::make_shared<GotoSEXP>(continuationIDX));
                            }
                            auto sinFlaseVertex = currCFGManager.addNode(sinFalseBlock);

                            // IRISEXP LBinop, IRISEXP RBinop, std::string OP
                            auto test = std::make_shared<BinopSEXP>(
                                std::make_shared<EnvReadSEXP>(
                                    std::make_shared<GlobalBindingSEXP>(targetName),
                                    false),
                                std::make_shared<EnvReadSEXP>(
                                    std::make_shared<GlobalBindingSEXP>(sinfo.guard),
                                    false),
                                "===");
                            // IRISEXP Test, bool NOT, double TRUE, double FALSE
                            bb->args.push_back(std::make_shared<IfElseJumpSEXP>(test, false, sinTrueIDX, sinFalseIDX));

                            currCFGManager.connect(bb, sinTrueBlock, {EdgeKind::Normal, stackReject});
                            currCFGManager.connect(bb, sinFalseBlock, {EdgeKind::Normal, stackReject});

                            currCFGManager.connect(sinTrueBlock, continuationBB, {EdgeKind::Normal, stackReject}); // TODO, goto to inlined code here
                            currCFGManager.connect(sinFalseBlock, continuationBB, {EdgeKind::Normal, stackReject});

                            break;
                          }
                          else
                          {
                            std::cout << "SinLining target has implicit bindings" << std::endl;
                          }
                        }
                        else
                        {
                          std::cout << "SinLining target has captured bindings" << std::endl;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }

#if PASSMGR_DEBUG == 1
      PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_START"));
#endif

      {
        DataflowSolver<ConstantsAtStmt> constantsAtStmtSolver(bbContView.cfgManager, true, [&]()
                                                              { return ConstantsAtStmt::bottom(); });
        for (auto &e : constantsAtStmtSolver.run(ConstantsAtStmt::boundary()))
        {
          ConstantProp::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
      }

#if PASSMGR_DEBUG == 1
      // DBG("End ConstantProp");
      PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_1_CONSTANT_PROP"));
#endif
      {
        DataflowSolver<CopyPropInfo> copyPropInfoSolver(bbContView.cfgManager, true, [&]()
                                                        { return CopyPropInfo(); });
        for (auto &e : copyPropInfoSolver.run(CopyPropInfo()))
        {
          auto currBB = bbContView.cfgManager.cfg[e.first];
          CopyProp::Transform(currBB, e.second);
        }
      }

#if PASSMGR_DEBUG == 1
      // DBG("End CopyProp");
      PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_2_COPY_PROP"));
#endif

      {
        WriteBarrierReduction::currFileView = &fileView;
        DataflowSolver<TDZA> tdzaSolver(bbContView.cfgManager, true, [&]()
                                        { return TDZA::bottom(allStackBindings); });
        for (auto &e : tdzaSolver.run(TDZA::boundary(allStackBindings)))
        {
          WriteBarrierReduction::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
      }

#if PASSMGR_DEBUG == 1
      // DBG("End WriteBarrierReduction");
      PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_3_WBR"));
#endif

      {
        DataflowSolver<Liveness> livenessSolver(bbContView.cfgManager, false, [&]()
                                                { return Liveness::bottom(); });
        for (auto &e : livenessSolver.run(Liveness::boundary(capturedStackBindings)))
        {
          auto currBB = bbContView.cfgManager.cfg[e.first];
          DCE::Transform(currBB, e.second);
          filterNOPs(currBB);
        }
      }

#if PASSMGR_DEBUG == 1
      // DBG("End DCE");
      PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_4_DCE"));
#endif

      {
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
      }

#if PASSMGR_DEBUG == 1
      // DBG("End Effect Prop");
      PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_5_EFFECT_PROP"));
#endif

      {
        // std::cout << "Starting SetSafePropKeyAccesses" << std::endl;
        DataflowSolver<SetSafePropKeyAccesses> setSafePropKeyAccesses(bbContView.cfgManager, true, [&]()
                                                                      { return SetSafePropKeyAccesses::bottom(uncapturedStackBindings); });

        for (auto &e : setSafePropKeyAccesses.run(SetSafePropKeyAccesses::boundary(uncapturedStackBindings)))
        {
          RemoveRedundantPropKeyCast::Transform(bbContView.cfgManager.cfg[e.first], e.second);
        }
      }

#if PASSMGR_DEBUG == 1
      // DBG("End SetSafePropKeyAccesses");
      PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_5_SAFE_PROP_KEY_ACCESS"));
#endif

      reduceComputedFieldOps(bbContView);

      doDeadBindingRemoval(fileView);
    }
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