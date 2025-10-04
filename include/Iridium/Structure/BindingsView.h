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
  std::vector<std::shared_ptr<RemoteEnvBindingSEXP>> remoteBindings;

  BindingsView(
      double root,
      IRISEXP tCont,
      std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext) : root(root), iridiumBuildContext(iridiumBuildContext)
  {
    targetContainer = std::dynamic_pointer_cast<BindingsSEXP>(tCont);
    assert(targetContainer);
    for (auto & rb : targetContainer->getRemoteBindings()->args)
    {
      auto remoteBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(rb);
      assert(remoteBinding);
      remoteBindings.push_back(remoteBinding);
    }
    initBindingsTree();
  }

  BindingsView(
      std::unordered_map<int, IRIBUILDCONTEXT> &iridiumBuildContext) : iridiumBuildContext(iridiumBuildContext)
  {
  }

  void initBindingsTree();
  
  void demoteArgumentsToRoot();

  void mergeBindingsTree(double scopeIdx, BindingsView & other);

  BindingsView clone(
    std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::shared_ptr<EnvBindingSEXP>> & localIndirectionMap, 
    std::unordered_map<std::shared_ptr<RemoteEnvBindingSEXP>, std::shared_ptr<RemoteEnvBindingSEXP>> & remoteIndirectionMap
  )
  {
    BindingsView res(iridiumBuildContext);
    res.prev = prev;
    res.next = next;
    res.root = root;
    
    // res.args = args;
    // res.bindings = bindings;
    // res.remoteBindings = remoteBindings; // this is not used rn, so just copy it as it is...

    for (auto & a : args) 
    {
      // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
      auto replacement = std::make_shared<EnvBindingSEXP>(
          a->getNAME(), a->hasASW(), a->hasJSARG(), a->hasJSRESTARG(), a->hasJSLET(), a->hasJSCONST(), a->hasJSVAR(), a->getIDX(), a->getREFIDX(), a->getScope(), a->getParentScope(), a->getNEXT()
        );
      res.args.push_back(
        replacement
      );
      localIndirectionMap[a] = replacement;
    }


    for (auto & e : bindings) 
    {
      for (auto & a : e.second)
      {
        // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
        auto replacement = std::make_shared<EnvBindingSEXP>(
            a->getNAME(), a->hasASW(), a->hasJSARG(), a->hasJSRESTARG(), a->hasJSLET(), a->hasJSCONST(), a->hasJSVAR(), a->getIDX(), a->getREFIDX(), a->getScope(), a->getParentScope(), a->getNEXT()
          );
        res.bindings[e.first].push_back(replacement);
        localIndirectionMap[a] = replacement;
      }
    }

    for (auto & rb : remoteBindings)
    {
      // IRISEXP ParentReference, bool NSIMPORT, double REFIDX
      auto replacement = std::make_shared<RemoteEnvBindingSEXP>(
        rb->getParentReference(), rb->hasNSIMPORT(), rb->getREFIDX()
      );
      remoteIndirectionMap[rb] = replacement;
    }

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