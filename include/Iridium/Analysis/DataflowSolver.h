#pragma once
#include "Iridium/Structure/CFGManager.h"
#include <sstream>
#include <functional>


template <typename Domain>
class DataflowSolver
{
public:
  DataflowSolver(CFG &cfg, Vertex entry, bool forward, std::function<Domain(void)> bottomInit)
      : cfg(cfg), entry(entry), forward(forward), bottomInitClosure(bottomInit) {}

  std::unordered_map<Vertex, Domain> run(const Domain &init)
  {
    std::unordered_map<Vertex, Domain> in, out;

    // initialize all blocks
    for (auto v : boost::make_iterator_range(boost::vertices(cfg)))
    {
      in[v] = bottomInitClosure();
      out[v] = bottomInitClosure();
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
          // {
          //   std::cout << std::endl << "SOLVERAT(BB): " << cfg[v]->getIDX() << std::endl;
          // }
          
          // in[v] = meet of predecessors' out
          newIn = (v == entry ? in[v] : bottomInitClosure());
          for (auto p : predecessors(v))
          {
            newIn = newIn.merge(out[p]);
          }
          in[v] = newIn;

          // {
          //   std::cout << "IN" << std::endl;
          //   std::ostringstream ss;
          //   newIn.dump(ss);
          //   std::cout << ss.str() << std::endl;
          // }

          // out[v] = transfer(in[v], block)
          newOut = in[v].transfer(cfg[v]);

          // {
          //   std::cout << "OUT" << std::endl;
          //   std::ostringstream ss;
          //   newOut.dump(ss);
          //   std::cout << ss.str() << std::endl;
          // }
        }
        else
        {
          // out[v] = meet of successors' in
          newOut = bottomInitClosure();
          for (auto s : successors(v))
          {
            newOut = newOut.merge(in[s]);
          }
          out[v] = newOut;

          // in[v] = transfer(out[v], block)
          newIn = out[v].transfer(cfg[v]);
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
  std::function<Domain(void)> bottomInitClosure;

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