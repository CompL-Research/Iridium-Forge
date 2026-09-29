#include "Support/ClosureTree.hpp"
#include "Storage/Config.h"
#include "Support/BBContainerSupport.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace IRI_STRUCTURAL {
using namespace IRI_STORAGE;

ClosureTree::ClosureTree(IRIContext &p, IRID rootNode) : ctx(p) {
  addClosure(rootNode);
}

void ClosureTree::addClosure(IRID id) {
  BBContainerSupport bbc(id, ctx);
  if (closures.contains(id)) {
    throw std::runtime_error("Tried to add a duplicate closure");
  }

  closures[id] = std::make_unique<IRICFG>(id, ctx);
  scopeIDXtoClosure[bbc.getScopeIDX()] = closures[id].get();
  bbIDXtoClosure[bbc.getStartBBIDX()] = closures[id].get();

  outEdges[id] = {};
  inEdges[id] = {};

  if (!root_id.has_value()) {
    root_id = id;
  }
}

void ClosureTree::addEdge(IRID from, IRID to) {
  if (!closures.contains(from) || !closures.contains(to)) {
    throw std::runtime_error("Source or destination IRID does not exist.");
  }

  if (root_id == to) {
    throw std::runtime_error("Cannot add an outgoing edge ");
  }

  outEdges[from].push_back(to);
  inEdges[to].push_back(from);
}

void ClosureTree::addEdgeFromScopeToBBIDX(double fromScope, double toBBIDX) {
  if (!scopeIDXtoClosure.contains(fromScope) ||
      !bbIDXtoClosure.contains(toBBIDX)) {
    throw std::runtime_error("Closure with startBBIDX does not exist");
  }
  addEdge(scopeIDXtoClosure[fromScope]->id, bbIDXtoClosure[toBBIDX]->id);
}

IRICFG *ClosureTree::getClosureByStartBBIDX(double startBBIDX) const {
  auto it = bbIDXtoClosure.find(startBBIDX);
  if (it == bbIDXtoClosure.end()) {
    throw std::runtime_error("Tried to get a non-existant closure");
  }
  return it->second;
}

void ClosureTree::commit() {
  for (auto &e : closures) {
    e.second->commit();
  }
}

// ============================================================================
// Access & Traversal
// ============================================================================

IRICFG *ClosureTree::getRoot() {
  if (root_id.has_value()) {
    return closures.at(root_id.value()).get();
  }
  throw std::runtime_error("Root not defined");
}

const IRICFG *ClosureTree::getRoot() const {
  if (root_id.has_value()) {
    return closures.at(root_id.value()).get();
  }
  throw std::runtime_error("Root not defined");
}

void ClosureTree::preorderTraversal(
    const std::function<void(IRICFG *)> &visitor) const {
  if (!root_id.has_value()) {
    throw std::runtime_error("Root not defined");
  }
  traverseRecursive(root_id.value(), visitor);
}

void ClosureTree::traverseRecursive(
    IRID current_id, const std::function<void(IRICFG *)> &visitor) const {
  // Visit the node
  visitor(closures.at(current_id).get());

  // Recurse down children
  for (IRID child_id : outEdges.at(current_id)) {
    traverseRecursive(child_id, visitor);
  }
}

// Public dump entry point
void ClosureTree::dumpFlat(std::ostream &oss) const {
  if (!root_id.has_value()) {
    throw std::runtime_error("Tree root not initialized");
  }
  oss << "\n=============================================\n";
  oss << "            ClosureTree STATE DUMP             \n";
  oss << "=============================================\n";

  IRI_STORAGE::IRID root = root_id.value();
  BBContainerSupport bbc(root, ctx);

  oss << "Root [BB" << bbc.getStartBBIDX() << "@" << bbc.getScopeIDX() << "]\n";
  // Fetch children of the root
  auto it = outEdges.find(root);
  if (it != outEdges.end()) {
    const auto &children = it->second;
    for (size_t i = 0; i < children.size(); ++i) {
      bool is_last = (i == children.size() - 1);
      dumpRecursive(oss, children[i], "", is_last);
    }
  }
}

// Private recursive dumper
void ClosureTree::dumpRecursive(std::ostream &oss, IRI_STORAGE::IRID current_id,
                                const std::string &prefix, bool is_last) const {
  BBContainerSupport bbc(current_id, ctx);
  // Print current node's branch character and ID
  oss << prefix << (is_last ? "└── " : "├── ") << "[BB" << bbc.getStartBBIDX()
      << "@" << bbc.getScopeIDX() << "]";

  oss << "\n";

  // Check if this node has children
  auto edges_it = outEdges.find(current_id);
  if (edges_it != outEdges.end()) {
    const auto &children = edges_it->second;

    // Calculate the prefix for the next level down
    std::string next_prefix = prefix + (is_last ? "    " : "│   ");

    for (size_t i = 0; i < children.size(); ++i) {
      bool child_is_last = (i == children.size() - 1);
      dumpRecursive(oss, children[i], next_prefix, child_is_last);
    }
  }
}

} // namespace IRI_STRUCTURAL
