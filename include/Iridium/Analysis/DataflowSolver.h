#pragma once
#include "Iridium/Structure/CFGManager.h"

template <typename Domain>
class DataflowSolver
{
public:
  DataflowSolver(CFG &cfg, Vertex entry, bool forward = true)
      : cfg(cfg), entry(entry), forward(forward) {}

  std::unordered_map<Vertex, Domain> run(const Domain &init)
  {
    std::unordered_map<Vertex, Domain> in, out;

    // initialize all blocks
    for (auto v : boost::make_iterator_range(boost::vertices(cfg)))
    {
      in[v] = Domain::top();
      out[v] = Domain::top();
    }

    // entry condition (often init != top)
    in[entry] = init;

    bool changed = true;
    while (changed)
    {
      changed = false;

      for (auto v : boost::make_iterator_range(boost::vertices(cfg)))
      {
        Domain newIn, newOut;

        if (forward)
        {
          // in[v] = meet of predecessors' out
          newIn = (v == entry ? in[v] : Domain::top());
          for (auto p : predecessors(v))
          {
            newIn = newIn.merge(out[p]);
          }
          in[v] = newIn;

          // out[v] = transfer(in[v], block)
          newOut = in[v].transfer(*cfg[v]);
        }
        else
        {
          // out[v] = meet of successors' in
          newOut = Domain::top();
          for (auto s : successors(v))
          {
            newOut = newOut.merge(in[s]);
          }
          out[v] = newOut;

          // in[v] = transfer(out[v], block)
          newIn = out[v].transfer(*cfg[v]);
        }

        // check change
        if (newOut == out[v] && newIn == in[v])
          continue;
        changed = true;
        in[v] = newIn;
        out[v] = newOut;
      }
    }

    return forward ? out : in; // return final solution
  }

private:
  CFG &cfg;
  Vertex entry;
  bool forward;

  std::vector<Vertex> predecessors(Vertex v)
  {
    std::vector<Vertex> preds;
    for (auto e : boost::make_iterator_range(boost::in_edges(v, cfg)))
    {
      preds.push_back(boost::source(e, cfg));
    }
    return preds;
  }

  std::vector<Vertex> successors(Vertex v)
  {
    std::vector<Vertex> succs;
    for (auto e : boost::make_iterator_range(boost::out_edges(v, cfg)))
    {
      succs.push_back(boost::target(e, cfg));
    }
    return succs;
  }
};