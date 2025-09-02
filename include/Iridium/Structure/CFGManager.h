#pragma once

#include "generated/IridiumTypes.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"
#include <unordered_map>
#include <queue>

// CFG
using CFG = boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::bidirectionalS,
    std::shared_ptr<BBSEXP>>;

using Vertex = boost::graph_traits<CFG>::vertex_descriptor;

class CFGManager
{
public:
  std::shared_ptr<ListSEXP> targetContainer;
  std::unordered_map<double, Vertex> bbIdxToVertex;

  explicit CFGManager() : entry(boost::graph_traits<CFG>::null_vertex()) {}

  // Add node
  Vertex addNode(std::shared_ptr<BBSEXP> bb, bool isEntryNode = false)
  {
    auto v = boost::add_vertex(bb, cfg);
    if (isEntryNode)
      entry = v;
    bbIdxToVertex[bb->getIDX()] = v;
    return v;
  }

  // Get node
  std::shared_ptr<BBSEXP> getNode(double bbIdx)
  {
    assert(bbIdxToVertex.find(bbIdx) != bbIdxToVertex.end() && "getNode failed");
    return getNode(bbIdxToVertex[bbIdx]);
  }

  std::shared_ptr<BBSEXP> getNode(Vertex id)
  {
    return cfg[id];
  }

  // Add edge
  void connect(std::shared_ptr<BBSEXP> from, std::shared_ptr<BBSEXP> to)
  {
    connect(from->getIDX(), to->getIDX());
  }

  void connect(double from, double to)
  {
    assert((bbIdxToVertex.find(from) != bbIdxToVertex.end()) && "connect failed 1");
    assert((bbIdxToVertex.find(to) != bbIdxToVertex.end()) && "connect failed 2");
    connect(bbIdxToVertex[from], bbIdxToVertex[to]);
  }

  void connect(Vertex from, Vertex to)
  {
    boost::add_edge(from, to, cfg);
  }

  // Get successors
  std::vector<Vertex> successors(Vertex v) const
  {
    std::vector<Vertex> succs;
    for (auto e : boost::make_iterator_range(boost::out_edges(v, cfg)))
    {
      succs.push_back(boost::target(e, cfg));
    }
    return succs;
  }

  // Get predecessors
  std::vector<Vertex> predecessors(Vertex v) const
  {
    std::vector<Vertex> preds;
    for (auto e : boost::make_iterator_range(boost::in_edges(v, cfg)))
    {
      preds.push_back(boost::source(e, cfg));
    }
    return preds;
  }

  void mergeSequentialBlocks(Vertex a, Vertex b)
  {
    assert(successors(a).size() == 1 && successors(a)[0] == b);
    assert(predecessors(b).size() == 1 && predecessors(b)[0] == a);

    // Step 1: merge B into A at the IR level
    auto & lastStmtinA = cfg[a]->args.back();
    assert(std::dynamic_pointer_cast<GotoSEXP>(lastStmtinA));
    cfg[a]->args.pop_back();

    for (auto & stmt : cfg[b]->args)
    {
      cfg[a]->args.push_back(stmt);
    }

    // Step 2: rewire edges from B to A
    auto succsB = successors(b);
    for (auto s : succsB)
    {
      boost::add_edge(a, s, cfg);
    }

    // Step 3: remove B
    double idxB = cfg[b]->getIDX();
    bbIdxToVertex.erase(idxB);
    boost::clear_vertex(b, cfg);
    boost::remove_vertex(b, cfg);
  }

  // Traverse CFG from entry
  template <typename Visitor>
  void traverseCFG(Visitor &&visit, bool depthFirst = true)
  {
    assert(entry != boost::graph_traits<CFG>::null_vertex() && "No entry node!");

    std::unordered_set<Vertex> visited;

    auto process = [&](auto &&self, Vertex v) -> void
    {
      if (visited.count(v))
        return;
      visited.insert(v);

      visit(v, cfg[v]); // give both vertex + BBSEXP to visitor

      for (auto succ : successors(v))
      {
        self(self, succ);
      }
    };

    if (depthFirst)
    {
      process(process, entry);
    }
    else
    {
      std::queue<Vertex> q;
      q.push(entry);
      while (!q.empty())
      {
        auto v = q.front();
        q.pop();
        if (visited.count(v))
          continue;
        visited.insert(v);

        visit(v, cfg[v]);
        for (auto succ : successors(v))
        {
          q.push(succ);
        }
      }
    }
  }

  void dumpCFGDOT(std::string filePath);

  // Print entire CFG
  void printCFG(std::ostream &os) const
  {
    for (auto v : boost::make_iterator_range(boost::vertices(cfg)))
    {
      cfg[v]->prettyPrint(os);
      auto succs = successors(v);
      for (auto s : succs)
      {
        os << "  -> BB" << cfg[s]->getIDX() << "\n";
      }
      os << "\n";
    }
  }

  Vertex entryBlock() const { return entry; }

  CFG cfg;

private:
  
  Vertex entry;
};