#include "Iridium/Structure/BindingsView.h"
#include "generated/IridiumTypes.h"
#include "Iridium/Structure/FileView.h"

BindingsView BindingsView::clone(
    std::unordered_map<std::shared_ptr<EnvBindingSEXP>, std::shared_ptr<EnvBindingSEXP>> &localIndirectionMap,
    std::unordered_map<std::shared_ptr<RemoteEnvBindingSEXP>, std::shared_ptr<RemoteEnvBindingSEXP>> &remoteIndirectionMap)
{
  BindingsView res(iridiumBuildContext);
  res.prev = prev;
  res.next = next;
  res.root = root;

  // res.args = args;
  // res.bindings = bindings;
  // res.remoteBindings = remoteBindings; // this is not used rn, so just copy it as it is...

  for (auto &a : args)
  {
    
    // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
    auto replacement = std::make_shared<EnvBindingSEXP>(
        a->getNAME(), a->hasASW(), a->hasJSARG(), a->hasJSRESTARG(), a->hasJSLET(), a->hasJSCONST(), a->hasJSVAR(), a->getIDX(), a->getREFIDX(), a->getScope(), a->getParentScope(), a->getNEXT());
    res.args.push_back(replacement);

    if (FileView::dynamicEvaledBindings.count(a) > 0)
    {
      FileView::dynamicEvaledBindings.insert(replacement);
    }

    localIndirectionMap[a] = replacement;
  }

  for (auto &e : bindings)
  {
    for (auto &a : e.second)
    {
      // std::string NAME, bool ASW, bool JSARG, bool JSRESTARG, bool JSLET, bool JSCONST, bool JSVAR, double IDX, double REFIDX, double Scope, double ParentScope, double NEXT
      auto replacement = std::make_shared<EnvBindingSEXP>(
          a->getNAME(), a->hasASW(), a->hasJSARG(), a->hasJSRESTARG(), a->hasJSLET(), a->hasJSCONST(), a->hasJSVAR(), a->getIDX(), a->getREFIDX(), a->getScope(), a->getParentScope(), a->getNEXT());
      res.bindings[e.first].push_back(replacement);
      localIndirectionMap[a] = replacement;
      if (FileView::dynamicEvaledBindings.count(a) > 0)
      {
        FileView::dynamicEvaledBindings.insert(replacement);
      }
    }
  }

  for (auto &rb : remoteBindings)
  {
    // IRISEXP ParentReference, bool NSIMPORT, double REFIDX
    auto replacement = std::make_shared<RemoteEnvBindingSEXP>(
        rb->getParentReference(), rb->hasNSIMPORT(), rb->getREFIDX());
    res.remoteBindings.push_back(replacement);
    remoteIndirectionMap[rb] = replacement;
  }

  res.targetContainer = targetContainer;

  return res;
}

void BindingsView::demoteArgumentsToRoot()
{
  for (auto &a : args)
  {
    a->unsetJSARG();
    a->unsetJSRESTARG(); // remove flags which indicate that these are argument bindings
    a->setJSVAR();
    bindings[root].push_back(a);
  }
  args.clear();
}

void BindingsView::mergeBindingsTree(double scopeIdx, BindingsView &other)
{
  // SCOPEIDX ---> other.root
  if (other.root != scopeIdx)
  {
    prev[other.root] = scopeIdx;
    next[scopeIdx].insert(other.root);
    for (auto &e : other.prev)
    {
      // assert(prev.count(e.first) == 0);
      prev[e.first] = e.second;
    }

    for (auto &e : other.next)
    {
      // assert(next.count(e.first) == 0);

      for (auto s : e.second)
      {
        next[e.first].insert(s);
      }
    }
  }

  // Copy over stack bindings
  for (auto &e : other.bindings)
  {
    for (auto &b : e.second)
    {
      bindings[e.first].push_back(b);
    }
  }

  // Copy over remote bindings
  for (auto &e : other.remoteBindings)
  {
    e->setREFIDX(remoteBindings.size());
    remoteBindings.push_back(e);
  }
}

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

  for (auto &a : args)
  {
    // // Restore flags, this can get affected if bindings were demoted inside a clone
    // a->setJSARG();
    // a->unsetJSLET();
    // a->unsetJSCONST();
    // a->unsetJSVAR();
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