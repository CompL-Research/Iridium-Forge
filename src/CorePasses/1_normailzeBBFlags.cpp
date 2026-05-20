#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include "Support/BBSupport.hpp"
#include <unordered_map>
#include <algorithm>

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

static inline bool mergeCTXWithAIScope(IridiumPool &pool, int curr,
                                       BUILD_CTX &ctx) {
  auto &buildContext = ctx[curr];
  IRI_GEN::BBSEXP startBB(buildContext->BB.at(0), pool);

  if (startBB.hasClosureBoundary() || startBB.hasTopLevel())
    return false;
  return true;
}

void collapseSubtree(int argInitNode, int parentNode, int currNode,
                     std::unordered_map<int, int> &outEdges,
                     std::unordered_map<int, std::vector<int>> &inEdges,
                     BUILD_CTX &iridiumBuildContext, IridiumPool &pool) {

  // 1. Copy the children to avoid iterator invalidation when modifying inEdges
  std::vector<int> children = inEdges[currNode];

  // 2. Check the collapse condition (skipping the root argInitNode itself)
  bool shouldCollapse = false;
  if (currNode != argInitNode) {
    shouldCollapse = mergeCTXWithAIScope(pool, currNode, iridiumBuildContext);
  }

  if (shouldCollapse) {
    // --- REWIRING PHASE ---

    // a. Rewire all immediate incoming edges (children) to argInitNode
    for (int child : children) {
      outEdges[child] = argInitNode;
      inEdges[argInitNode].push_back(child);

      // b. Update the context parent pointer
      iridiumBuildContext[child]->parent = argInitNode;
    }

    // c. Remove currNode from its original parent's inEdges list
    auto &parentChildren = inEdges[parentNode];
    parentChildren.erase(
        std::remove(parentChildren.begin(), parentChildren.end(), currNode),
        parentChildren.end());

    // d. Clean up the collapsed node from the maps
    inEdges.erase(currNode);
    outEdges.erase(currNode);

    // e. Copy over BBs from the removed node into argInitScope
    auto &aiBBs = iridiumBuildContext[argInitNode]->BB;
    auto &bbsToMerge = iridiumBuildContext[currNode]->BB;
    for (auto & x : bbsToMerge) {
      IRI_STRUCTURAL::BBSupport bb(x, pool);
      bb.setScopeIDX(argInitNode);
    }
    aiBBs.insert(aiBBs.end(), bbsToMerge.begin(), bbsToMerge.end());

    // 1. Create a weak_ptr observer. This does NOT increase the reference count.
    std::weak_ptr<IridiumBuildContext> observer = iridiumBuildContext[currNode];

    // 2. Erase the node. This destroys the shared_ptr held by the map.
    iridiumBuildContext.erase(currNode);

    // 3. Assert that the memory was actually freed.
    // weak_ptr::expired() returns true if the reference count hit 0.
    assert(observer.expired() && "Memory leak: Another shared_ptr is still holding this context!");

    // e. TERMINATE RECURSION
    return;
  }

  // --- NO COLLAPSE PHASE ---

  // If the node didn't collapse, we continue traversing further down the tree.
  for (int child : children) {
    collapseSubtree(argInitNode, currNode, child, outEdges, inEdges,
                    iridiumBuildContext, pool);
  }
}

static inline IRI_FLAG getBBFlag(BBSEXP &b) {
  if (b.hasTopLevel())
    return IRI_FLAG::TopLevel;
  if (b.hasClosureBoundary())
    return IRI_FLAG::ClosureBoundary;
  if (b.hasLexical())
    return IRI_FLAG::Lexical;
  if (b.hasVARBoundary())
    return IRI_FLAG::VARBoundary;
  throw std::runtime_error("Failed to get a valid flag from a BBSEXP");
}

static inline void setBBFlag(BBSEXP &b, IRI_FLAG flagToSet) {
  b.clearTopLevel();
  b.clearClosureBoundary();
  b.clearLexical();
  b.clearVARBoundary();
  if (flagToSet == IRI_GEN::TopLevel)
    return b.setTopLevel();
  if (flagToSet == IRI_GEN::ClosureBoundary)
    return b.setClosureBoundary();
  if (flagToSet == IRI_GEN::Lexical)
    return b.setLexical();
  if (flagToSet == IRI_GEN::VARBoundary)
    return b.setVARBoundary();
  throw std::runtime_error("Impossible case reached setBBFlag");
}


// Recursive helper to print the tree with indentation
void printTreeHelper(int node, const std::unordered_map<int, std::vector<int>>& inEdges, int depth, BUILD_CTX & iridiumBuildContext) {
    // Create indentation based on depth
    std::string indent(depth * 4, ' ');
    if (node >= 0 && iridiumBuildContext[node]->loopConfig) {
      auto lc = iridiumBuildContext[node]->loopConfig.value();
      std::cout << indent << "|-- " << node << "[LOOP_CTX] {" << (lc.label ? lc.label.value() : "") << "}\n";
    } else {
      std::cout << indent << "|-- " << node << "\n";
    }

    // Find and print all children
    auto it = inEdges.find(node);
    if (it != inEdges.end()) {
        for (int child : it->second) {
            printTreeHelper(child, inEdges, depth + 1, iridiumBuildContext);
        }
    }
}

// Main method to find roots and trigger the printing
void printTree(const std::unordered_map<int, int>& outEdges,
               const std::unordered_map<int, std::vector<int>>& inEdges, BUILD_CTX &iridiumBuildContext) {

    // 1. Find the root(s)
    // A root is a node that acts as a parent (exists in inEdges)
    // but has no parent itself (does not exist as a key in outEdges).
    std::vector<int> roots;
    for (const auto& pair : inEdges) {
        int node = pair.first;
        if (outEdges.find(node) == outEdges.end()) {
            roots.push_back(node);
        }
    }

    if (roots.empty()) {
        std::cout << "Tree is empty or contains a cycle with no clear root.\n";
        return;
    }

    // 2. Print each root (handles forests if there are disconnected trees)
    for (int root : roots) {
        std::cout << "Tree rooted at " << root << ":\n";
        printTreeHelper(root, inEdges, 0, iridiumBuildContext);
        std::cout << "\n";
    }
}

void _1_NBBF(IridiumPool &pool, IRID sexp, BUILD_CTX &iridiumBuildContext) {
  std::unordered_map<int, int> outEdges;
  std::unordered_map<int, std::vector<int>> inEdges;
  std::vector<int> argInitScopes;
  //
  // Build a scope tree... this is messy, we do this again later
  // but for simplicity its a small reimplementation...
  //
  for (auto &e : iridiumBuildContext) {
    auto &buildContext = e.second;

    int currScope = e.first;
    int parentScope = buildContext->parent;
    outEdges[currScope] = parentScope;
    inEdges[parentScope].push_back(currScope);

    if (buildContext->isArgInitContext)
      argInitScopes.push_back(currScope);
  }

  // printTree(outEdges, inEdges, iridiumBuildContext);

  //
  // Collapse all scopes inside ArgInitScope [excluding boundaries of course]
  //
  for (auto &aiScope : argInitScopes) {
    collapseSubtree(aiScope, aiScope, aiScope, outEdges, inEdges, iridiumBuildContext, pool);
  }

  for (auto &e : iridiumBuildContext) {
    std::shared_ptr<IridiumBuildContext> buildContext = e.second;

    int currScope = e.first;
    int parentScope = buildContext->parent;
    outEdges[currScope] = parentScope;
    inEdges[parentScope].push_back(currScope);

    if (buildContext->isArgInitContext)
      argInitScopes.push_back(currScope);

    BBSEXP firstBB(buildContext->BB[0], pool);

    IRI_FLAG mainBBFlag = getBBFlag(firstBB);
    for (auto &b : buildContext->BB) {
      BBSEXP currBB(b, pool);
      setBBFlag(currBB, mainBBFlag);
      double currIDX = currBB.getIDX();
      if (pool.lastBBIDX < currIDX) {
        pool.lastBBIDX = currIDX;
      }
    }
  }
}
}; // namespace IRI_CORE_PASSES
