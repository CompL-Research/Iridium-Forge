#include "Iridium/Structure/CFGManager.h"
#include "Iridium/Cloning.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"
#include "external/graph-boost-1.89.0/graph_traits.hpp"
#include "external/graph-boost-1.89.0/iteration_macros.hpp"
#include <filesystem>

CFGManager CFGManager::clone(
  std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::shared_ptr<EnvBindingSEXP>> & localIndirectionMap, 
  std::unordered_map<std::shared_ptr<RemoteEnvBindingSEXP>, std::shared_ptr<RemoteEnvBindingSEXP>> & remoteIndirectionMap
)
{
  CFGManager res(iridiumBuildContext);
  res.targetContainer = targetContainer;
  res.bbIdxToVertex = bbIdxToVertex;
  res.cfg = cfg;
  res.entry = entry;
  // Clone basic blocks

  for (auto &e : bbIdxToVertex)
  {
    res.cfg[e.second] = cloneBB(cfg[e.second], localIndirectionMap, remoteIndirectionMap);
  }

  return res;
}

// DFS with memoization, handles multiple exits
int dfsHeight(const CFG &g, Vertex u,
              std::unordered_map<Vertex, int> &memo,
              std::unordered_set<Vertex> &visiting)
{
  if (memo.count(u))
    return memo[u];
  if (visiting.count(u))
    return 0; // break cycle

  // if u is an exit (no successors)
  if (out_degree(u, g) == 0)
  {
    return memo[u] = 0;
  }

  visiting.insert(u);

  int best = 0;
  for (auto [ei, ei_end] = boost::out_edges(u, g); ei != ei_end; ++ei)
  {
    Vertex v = boost::target(*ei, g);
    best = std::max(best, 1 + dfsHeight(g, v, memo, visiting));
  }

  visiting.erase(u);
  return memo[u] = best;
}

std::unordered_map<Vertex, int> computeHeights(const CFG &g, Vertex entry)
{
  std::unordered_map<Vertex, int> memo;
  std::unordered_map<Vertex, int> result;
  std::unordered_set<Vertex> visiting;

  BGL_FORALL_VERTICES(v, g, CFG)
  {
    result[v] = dfsHeight(g, v, memo, visiting);
  }

  return result;
}

std::vector<IRISEXP> CFGManager::chapati()
{
#if IRIDIUM_DUMP_FLATTENING_CFG == 1
  int step = 0;
  std::string debugPathPrefix = IRIDIUM_OUTPUTS_FOLDER + std::to_string((int)cfg[entry]->getIDX()) + "/";
  std::filesystem::path dir = debugPathPrefix;
  try
  {
    if (!std::filesystem::exists(dir))
    {
      std::filesystem::create_directories(dir);
    }
  }
  catch (const std::filesystem::filesystem_error &e)
  {
    std::cerr << "[IRIDIUM_DUMP_FLATTENING_CFG] Filesystem error: " << e.what() << '\n';
  }
#endif
  std::unordered_map<Vertex, int> heightMap = computeHeights(cfg, entry);

  std::set<Vertex> processed;

  auto safeToMerge = [&](Vertex v, std::shared_ptr<BBSEXP> currBB)
  {
    if (predecessors(v).size() == 1)
    {
      auto lastStmt = currBB->args.back();

      if (auto gotoStmt = std::dynamic_pointer_cast<GotoSEXP>(lastStmt))
      {
        double targetBBIdx = gotoStmt->getIDX();
        if (cfg[v]->getIDX() == targetBBIdx)
          return true;
      }
      else if (auto ifElseStmt = std::dynamic_pointer_cast<IfElseJumpSEXP>(lastStmt))
      {
        double targetTrueIdx = ifElseStmt->getTRUE();
        double targetFalseIdx = ifElseStmt->getFALSE();
        if (cfg[v]->getIDX() == targetTrueIdx)
          return true;
        if (cfg[v]->getIDX() == targetFalseIdx)
          return true;
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
      else if (auto throwStmt = std::dynamic_pointer_cast<ThrowSEXP>(lastStmt))
      {
        // TODO, retSEXP
      }
      else
      {
        throw std::runtime_error("Invalid LastNode when merge check of BB: " + lastStmt->tag);
      }
    }

    return false;
  };

  std::function<void(Vertex)> chap = [&](Vertex curr)
  {
#if IRIDIUM_DUMP_FLATTENING_CFG == 1
    dumpCFGDOT(debugPathPrefix + "step_" + std::to_string(step++) + "_BB" + std::to_string((int)cfg[curr]->getIDX()) + ".DOT");
#endif
    if (processed.find(curr) != processed.end())
      return;

    processed.insert(curr);

    auto _succs = successors(curr);

    std::set<Vertex> X;

    for (auto &s : _succs)
    {
      if (safeToMerge(s, cfg[curr]))
      {
        X.insert(s);
      }
    }

    if (X.size() == 0)
    {

      for (auto &s : _succs)
      {
        chap(s);
      }
      return;
    }

    // Atleast one safe to merge node
    assert(X.size() <= 2);

    // Heuristic, pick the one with the smallest height... probably more thought and work can be put here imo...
    Vertex largestByHeight = *std::max_element(
        X.begin(),
        X.end(),
        [&](Vertex a, Vertex b)
        {
          return heightMap[a] < heightMap[b];
        });
    mergeBlocks(curr, largestByHeight);

    processed.erase(curr);

    chap(curr);

    for (auto &s : successors(curr))
    {
      chap(s);
    }
  };

  chap(entry);

  std::vector<IRISEXP> result;

  traverseCFG(
      [&](Vertex v, std::shared_ptr<BBSEXP> bb)
      {
        result.push_back(cfg[v]);
      });

  // remove reduntant gotos
  for (size_t i = 0; i < result.size(); i++)
  {
    auto bb = std::dynamic_pointer_cast<BBSEXP>(result[i]);
    assert(bb);

    auto lastStmtOfCurrentBB = bb->args[bb->args.size() - 1];

    if (auto gotoStmt = std::dynamic_pointer_cast<GotoSEXP>(lastStmtOfCurrentBB))
    {
      if (i + 1 < result.size())
      {
        auto nextBB = std::dynamic_pointer_cast<BBSEXP>(result[i + 1]);
        assert(nextBB);

        if (gotoStmt->getIDX() == nextBB->getIDX())
        {
          bb->args.pop_back();
        }
      }
    }
  }

  bool decorateBBEntries = false;

  if (decorateBBEntries)
  {
    for (auto & b : result)
    {
      auto bb = std::dynamic_pointer_cast<BBSEXP>(b);
      assert(bb);
      {
        auto cs = std::make_shared<CallSiteSEXP>(false, false, false, false, false, false, false, false);
        cs->unsetJSDirectEval();

        cs->args.push_back(
            std::make_shared<FieldReadSEXP>(
                std::make_shared<EnvReadSEXP>(
                    std::make_shared<GlobalBindingSEXP>("console"),
                    false),
                std::make_shared<StringSEXP>("log")));

        cs->args.push_back(
            std::make_shared<StringSEXP>("Entering BB" + std::to_string(bb->getIDX())));

        auto o = std::make_shared<StackRejectSEXP>(1);
        o->args.push_back(cs);
        
        bb->args.insert(bb->args.begin(), o);        
      }
    }
  }


  return result;
}