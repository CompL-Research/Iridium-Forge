#include "Iridium/PassManager.h"

#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/Domains/EffectAtStmt.h"
#include "Iridium/Analysis/Domains/CopyPropInfo.h"
#include "Iridium/Analysis/DataflowSolver.h"
#include "Iridium/OptimizationPasses/ConstantProp.h"
#include "Iridium/OptimizationPasses/WriteBarrierReduction.h"
#include "Iridium/OptimizationPasses/CopyProp.h"
#include "Iridium/OptimizationPasses/PropEffects.h"
#include "Iridium/OptimizationPasses/DCE.h"
#include "Iridium/OptimizationPasses/DeadBindingRemoval.h"
#include "Iridium/CorePasses/3_filterNops.h"
#include "external/json.hpp"
#include <fstream>

#include <filesystem>

using json = nlohmann::json;


void PassManager::optimize(int level)
{
  #if PASSMGR_DEBUG == 1
  std::filesystem::path out_path = "outputs/passmanager.json";
  json PASSMGR_DEBUGInfo = {
    {"data", json::array()} 
  };
  std::vector<json> PASSMGR_DEBUGVector;
  #endif


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
      EffectAtStmt::blacklist = capturedStackBindings;

      std::string passBasename = "Iter"+std::to_string((int)i) + "_BB" + std::to_string((int)bbContView.getStartBBIDX());

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
      PASSMGR_DEBUGVector.push_back(bbContView.getDebugJSON(passBasename + "_5_EFFECT_PROP"));
      #endif

      doDeadBindingRemoval(fileView);
    }
  }

  #if PASSMGR_DEBUG == 1
  std::filesystem::create_directories(out_path.parent_path());

  for (const auto& item : PASSMGR_DEBUGVector) {
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