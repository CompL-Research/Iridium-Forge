#include "Iridium/Structure/BindingsView.h"
#include "generated/IridiumTypes.h"

void BindingsView::initBindingsTree()
{
  bindings[root] = std::vector<std::shared_ptr<EnvBindingSEXP>>();

  std::function<void(double)> initPath = [&](double curr)
  {
    assert(curr != -1);
    if (curr == root)
      return;
    if (bindings.count(curr) == 0)
      bindings[curr] = std::vector<std::shared_ptr<EnvBindingSEXP>>();

    double parent = iridiumBuildContext[curr]->parent;
    prev[curr] = parent;
    next[parent].insert(curr);
    initPath(parent);
  };

  auto localBindings = std::dynamic_pointer_cast<ListSEXP>(targetContainer->getLocalBindings());
  assert(localBindings);
  for (auto &e : localBindings->args)
  {
    auto currEnvBinding = std::dynamic_pointer_cast<EnvBindingSEXP>(e);
    assert(currEnvBinding);
    if (currEnvBinding->hasJSARG() || currEnvBinding->hasJSRESTARG())
    {
      args.push_back(currEnvBinding);
      continue;
    }
    auto localScope = currEnvBinding->getScope();
    initPath(localScope);
    bindings[localScope].push_back(currEnvBinding);
  }
}

void BindingsView::removeBinding(std::shared_ptr<EnvBindingSEXP> binding)
{
  // Remove from local stack frame
  for (auto &e : bindings)
  {
    auto &bindingsVec = e.second;
    // Remove all even numbers
    bindingsVec.erase(
        std::remove_if(bindingsVec.begin(), bindingsVec.end(),
                       [&](std::shared_ptr<EnvBindingSEXP> elem)
                       { return elem == binding; }),
        bindingsVec.end());
  }

  auto &remoteBindingsVec = targetContainer->getRemoteBindings()->args;

  // Remove from remote binding frame
  remoteBindingsVec.erase(
      std::remove_if(remoteBindingsVec.begin(), remoteBindingsVec.end(),
                     [&](IRISEXP elem)
                     {
                       auto remBinding = std::dynamic_pointer_cast<RemoteEnvBindingSEXP>(elem);
                       assert(remBinding);

                       return resolveRemoteBinding(remBinding) == binding;
                     }),
      remoteBindingsVec.end());
}

std::shared_ptr<BindingsSEXP> BindingsView::checkout()
{
  std::vector<IRISEXP> res;
  auto localBindings = targetContainer->getLocalBindings();

  for (auto & a : args)
  {
    res.push_back(a);
  }

  auto refIdx = 0;
  std::function<void(double)> doPreOrderTraversal = [&](double currIdx)
  {
    for (auto &e : bindings[currIdx])
    {
      e->setNEXT(refIdx - 1);
      e->setREFIDX(refIdx);
      res.push_back(e);
      refIdx++;
    }

    for (auto n : next[currIdx])
    {
      doPreOrderTraversal(n);
    }
  };

  doPreOrderTraversal(root);

  std::function<double(double)> findNext = [&](double idx)
  {
    if (prev.count(idx) > 0)
    {
      auto lookupBindings = bindings[prev[idx]];
      if (lookupBindings.size() > 0)
      {
        return lookupBindings.back()->getREFIDX();
      }
      else if (prev.count(prev[idx]) > 0)
        return findNext(prev[idx]); // recurse
      else
        return (double)-1;
    }
    return (double)-1;
  };

  for (auto e : bindings)
  {

    if (e.second.size() > 0)
    {
      // Set next for the first element of each scope...
      e.second[0]->setNEXT(findNext(e.first));
    }
  }

  localBindings->args = std::move(res);
  return targetContainer;
}