#pragma once

#include "generated/IridiumTypes.h"
#include "external/graph-boost-1.89.0/adjacency_list.hpp"
#include <unordered_map>
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

private:
  CFG cfg;
  Vertex entry;
};