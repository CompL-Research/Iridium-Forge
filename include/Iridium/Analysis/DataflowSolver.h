#pragma once
#include "Iridium/Structure/CFGManager.h"
#include <sstream>
#include <functional>


template <typename Domain>
class DataflowSolver
{
public:
  DataflowSolver(CFGManager &manager, bool forward, std::function<Domain(void)> bottomInit)
      : cfgManager(manager), forward(forward), bottomInitClosure(bottomInit) {}

  std::unordered_map<Vertex, Domain> run(const Domain &init)
  {
    std::unordered_map<Vertex, Domain> in, out;

    Vertex entry = cfgManager.entryBlock();

    // initialize all blocks
    for (auto v : boost::make_iterator_range(boost::vertices(cfgManager.cfg)))
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

      for (auto v : boost::make_iterator_range(boost::vertices(cfgManager.cfg)))
      {
        Domain newIn, newOut;

        if (forward)
        {
          // {
          //   std::cout << std::endl << "SOLVERAT(BB): " << cfgManager.cfg[v]->getIDX() << std::endl;
          // }
          
          // in[v] = meet of predecessors' out
          newIn = (v == entry ? in[v] : bottomInitClosure());
          for (auto p : cfgManager.predecessors(v))
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
          newOut = in[v].transfer(cfgManager.cfg[v]);

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
          for (auto s : cfgManager.successors(v))
          {
            newOut = newOut.merge(in[s]);
          }
          out[v] = newOut;

          // in[v] = transfer(out[v], block)
          newIn = out[v].transfer(cfgManager.cfg[v]);
        }

        // check change
        if (newOut == out[v] && newIn == in[v])
          continue;
        changed = true;
        in[v] = newIn;
        out[v] = newOut;
      }
    }

    // return forward ? out : in; // return final solution
    return forward ? in : out; // return final solution
  }

private:
  CFGManager &cfgManager;
  bool forward;
  std::function<Domain(void)> bottomInitClosure;

};