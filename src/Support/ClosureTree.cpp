#include "Support/ClosureTree.hpp"
#include "Helpers.h"
#include "Storage/Config.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/FileSupport.hpp"
#include "Support/IRIS.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace IRI_STRUCTURAL {
using namespace IRI_STORAGE;

ClosureTree::ClosureTree(IRIContext &p, IRID fileSEXP) : ctx(p) {
  if (!ctx.iris)
    throw std::runtime_error(
        "ClosureTree: IRIS must exist before the closure tree is built");

  IRID rootContainer = IRI_HELPERS::getTopLevelContainer(ctx, fileSEXP);

  std::unordered_map<IRID, CIDX> byContainer;

  // Add root first so it always gets id 0
  addClosure(rootContainer, byContainer);

  FileSupport fs(fileSEXP, ctx);
  for (auto [bbcID, _] : fs.containers()) {
    if (bbcID == rootContainer)
      continue;
    addClosure(bbcID, byContainer);
  }
  root_id = 0;

  linkClosures(byContainer);
  relayout();
  resolveClosureRefs();
}

void ClosureTree::addClosure(IRID container,
                             std::unordered_map<IRID, CIDX> &byContainer) {
  if (byContainer.contains(container))
    throw std::runtime_error("Tried to add a duplicate closure");

  byContainer[container] = (CIDX)nodes.size();

  ClosureNode n;
  n.container = container;
  n.cfg = std::make_unique<IRICFG>(container, ctx);
  nodes.push_back(std::move(n));
}

void ClosureTree::linkClosures(
    const std::unordered_map<IRID, CIDX> &byContainer) {
  SCPIDX topLevel = ctx.iris->getTopLevelScope();

  for (CIDX h = 0; h < (CIDX)nodes.size(); h++) {
    SCPIDX scope = BBContainerSupport(nodes[h].container, ctx).getScopeIDX();
    if (scope == topLevel)
      continue;

    SCPIDX parent =
        ctx.iris->getEnclosingClosureScope(ctx.iris->parentScope(scope));
    if (parent == NO_SCOPE)
      throw std::runtime_error(
          "ClosureTree: closure scope has no enclosing closure: " +
          std::to_string(scope));

    CIDX p = byContainer.at(ctx.iris->containerForScope(parent));
    nodes[h].parent = p;
    nodes[p].children.push_back(h);
  }
}

void ClosureTree::relayout() {
  std::vector<CIDX> order;
  std::vector<CIDX> ends(nodes.size());

  std::function<void(CIDX)> visit = [&](CIDX h) {
    CIDX self = (CIDX)order.size();
    order.push_back(h);
    for (CIDX c : nodes[h].children)
      visit(c);
    ends[self] = (CIDX)order.size();
  };
  visit(root_id);

  if (order.size() != nodes.size())
    throw std::runtime_error(
        "ClosureTree: closure is not reachable from the root");

  std::vector<CIDX> oldToNew(nodes.size());
  for (CIDX i = 0; i < (CIDX)order.size(); i++)
    oldToNew[order[i]] = i;

  std::vector<ClosureNode> laid(order.size());
  for (CIDX i = 0; i < (CIDX)order.size(); i++) {
    ClosureNode &n = nodes[order[i]];
    laid[i].container = n.container;
    laid[i].cfg = std::move(n.cfg);
    laid[i].subtreeEnd = ends[i];
    laid[i].parent = n.parent == NO_CIDX ? NO_CIDX : oldToNew[n.parent];
    for (CIDX c : n.children)
      laid[i].children.push_back(oldToNew[c]);
  }

  nodes = std::move(laid);
  root_id = 0;

  preorder_.clear();
  preorder_.reserve(nodes.size());
  for (auto &n : nodes)
    preorder_.push_back(n.cfg.get());
}

void ClosureTree::resolveClosureRefs() {
  // Scope -> closure handle: how a site's ClosureScope becomes a ClosIDX.
  std::unordered_map<SCPIDX, CIDX> byScope;
  for (CIDX h = 0; h < (CIDX)nodes.size(); h++) {
    BBContainerSEXP container(nodes[h].container, ctx);
    container.setClosIDX(h);
    byScope[container.getScopeIDX()] = h;
  }

  std::function<void(IRID, CIDX)> patchNode = [&](IRID node, CIDX h) {
    for (IRID ref : ctx.storage.nodes.get_args_view(node)) {
      if (IRI_NODE(ctx, ref).tag == IRI_GEN::LambdaRef) {
        LambdaRefSEXP lref(ref, ctx);
        IRID lambda = lref.getArg_Lambda();
        LambdaSEXP lmb(lambda, ctx);

        if (!lmb.hasClosureScope())
          throw std::runtime_error("ClosureTree: Lambda has no ClosureScope: " +
                                   std::to_string(lambda));

        auto target = byScope.find(lmb.getClosureScope());
        if (target == byScope.end())
          throw std::runtime_error("ClosureTree: Lambda targets a closure "
                                   "outside this file: scope " +
                                   std::to_string(lmb.getClosureScope()));

        CIDX closure = target->second;
        if (closure != root_id && nodes[closure].parent != h)
          throw std::runtime_error(
              "ClosureTree: closure " + std::to_string(closure) +
              " is materialized " + "by " + std::to_string(h) +
              ", but its "
              "parent is " +
              std::to_string(nodes[closure].parent));

        auto [first, isNew] = canonical.try_emplace(closure, lambda);
        if (isNew)
          lmb.setClosIDX(closure);
        else if (first->second != lambda)
          lref.setArg_Lambda(first->second);
      }

      patchNode(ref, h);
    }
  };

  for (CIDX h = 0; h < (CIDX)nodes.size(); h++)
    for (BBIDX b = 0; b < nodes[h].cfg->numBBs(); b++) {
      const IRIBB &bb = nodes[h].cfg->at(b);
      for (IRID st : bb.stmts)
        patchNode(st, h);
      patchNode(bb.terminal, h);
    }

  for (CIDX h = 0; h < (CIDX)nodes.size(); h++) {
    if (h == root_id || canonical.contains(h))
      continue;

    // Create dummies for closures which were never instantiated...
    BBContainerSEXP target(nodes[h].container, ctx);
    canonical[h] = LambdaSEXP::create(ctx, target.getScopeIDX(), h, 0);
  }
}

void ClosureTree::populateSlots(CIDX h) {
  // Create the lambdas list for the logical frame
  std::vector<IRID> lambdas;
  for (size_t i = 0; i < nodes[h].children.size(); i++) {
    IRID lambda = canonical.at(nodes[h].children[i]);
    LambdaSEXP(lambda, ctx).setREFIDX(i);
    lambdas.push_back(lambda);
  }

  BBContainerSupport bbc(nodes[h].container, ctx);
  BindingsSEXP bindings(bbc.getArg_Bindings(), ctx);
  ctx.storage.nodes.set_args(bindings.getArg_Lambdas(), lambdas);
}

void ClosureTree::commit() {
  for (CIDX h = 0; h < (CIDX)nodes.size(); h++) {
    // Maybe experiment with more interesting orders later...
    nodes[h].cfg->commit(nodes[h].cfg->reversePostOrder());
    populateSlots(h);
  }
}

} // namespace IRI_STRUCTURAL
