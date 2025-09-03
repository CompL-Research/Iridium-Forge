#include "Iridium/Structure/CFGManager.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"
#include "external/graph-boost-1.89.0/graph_traits.hpp"
#include "external/graph-boost-1.89.0/iteration_macros.hpp"

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

  int step = 0;
  std::function<void(Vertex)> chap = [&](Vertex curr)
  {
    dumpCFGDOT("mergeDump/ENTRY" + std::to_string(reinterpret_cast<uintptr_t>(targetContainer.get())) + "_step_" + std::to_string(step++) + "_BB" + std::to_string((int)cfg[curr]->getIDX()) + ".DOT");
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

  return result;
}