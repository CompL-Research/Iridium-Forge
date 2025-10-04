#pragma once
#include "generated/IridiumTypes.h"
#include "Iridium/Globals.h"

class BindingsView
{
public:
  std::unordered_map<double, double> prev;
  std::unordered_map<double, std::set<double>> next;

  double root;
  std::vector<std::shared_ptr<EnvBindingSEXP>> args;
  std::unordered_map<double, std::vector<std::shared_ptr<EnvBindingSEXP>>> bindings;
  IRISEXP remoteBindings;

  BindingsView(
      double root,
      IRISEXP tCont,
      std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext) : root(root), iridiumBuildContext(iridiumBuildContext)
  {
    targetContainer = std::dynamic_pointer_cast<BindingsSEXP>(tCont);
    assert(targetContainer);
    remoteBindings = targetContainer->getRemoteBindings();
    initBindingsTree();
  }

  BindingsView(
      std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext) : iridiumBuildContext(iridiumBuildContext)
  {
  }

  void initBindingsTree();
  
  void demoteArgumentsToRoot();

  void mergeBindingsTree(double scopeIdx, BindingsView & other);

  BindingsView clone()
  {
    BindingsView res(iridiumBuildContext);
    res.prev = prev;
    res.next = next;
    res.root = root;
    res.args = args;
    res.bindings = bindings;
    res.remoteBindings = remoteBindings;
    res.targetContainer = targetContainer;

    return res;
  }

  void removeBinding(std::shared_ptr<EnvBindingSEXP> binding);

  /// Debug dump
  void dump(std::ostream &os = std::cout) const
  {
    os << "=== BindingsView Dump ===\n";
    os << "Root: " << root << "\n\n";

    os << "[Prev Map]\n";
    for (auto &p : prev)
    {
      os << "  " << p.first << " -> " << p.second << "\n";
    }
    if (prev.empty())
      os << "  (empty)\n";
    os << "\n";

    os << "[Next Map]\n";
    for (auto &p : next)
    {
      os << "  " << p.first << " -> {";
      bool first = true;
      for (auto &v : p.second)
      {
        if (!first)
          os << ", ";
        os << v;
        first = false;
      }
      os << "}\n";
    }
    if (next.empty())
      os << "  (empty)\n";
    os << "\n";

    os << "[Bindings]\n";
    for (auto &b : bindings)
    {
      os << "  " << b.first << " -> [";
      bool first = true;
      for (auto &binding : b.second)
      {
        if (!first)
          os << ", ";
        if (binding)
        {
          os << "EnvBindingSEXP@" << binding->getNAME();
        }
        else
        {
          os << "null";
        }
        first = false;
      }
      os << "]\n";
    }
    if (bindings.empty())
      os << "  (empty)\n";
    os << "\n";

    os << "TargetContainer: "
       << (targetContainer ? "BindingsSEXP@" + std::to_string((uintptr_t)targetContainer.get())
                           : "null")
       << "\n";
    os << "=========================\n";
  }

  std::shared_ptr<BindingsSEXP> checkout();

private:
  std::shared_ptr<BindingsSEXP> targetContainer;
  std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext;
};