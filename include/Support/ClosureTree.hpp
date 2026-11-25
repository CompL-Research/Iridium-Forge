#pragma once

#include "IRICFG.hpp"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include <memory>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace IRI_STRUCTURAL {

struct ClosureNode {
  IRI_STORAGE::IRID container;
  std::unique_ptr<IRICFG> cfg;
  CIDX parent = NO_CIDX;
  CIDX subtreeEnd = NO_CIDX;
  std::vector<CIDX> children;
};

class ClosureTree {
public:
  ClosureTree(IRI_STORAGE::IRIContext &, IRI_STORAGE::IRID fileSEXP);

  const std::vector<IRICFG *> &preorder() const { return preorder_; }

  IRICFG *root() const { return nodes[root_id].cfg.get(); }
  size_t size() const { return nodes.size(); }
  const ClosureNode &node(CIDX h) const { return nodes[h]; }

  void commit();

private:
  void addClosure(IRI_STORAGE::IRID container,
                  std::unordered_map<IRI_STORAGE::IRID, CIDX> &byContainer);
  void
  linkClosures(const std::unordered_map<IRI_STORAGE::IRID, CIDX> &byContainer);
  void relayout();
  void resolveClosureRefs();
  void populateSlots(CIDX h);
  void dumpNode(std::ostream &oss, CIDX h, const std::string &prefix,
                bool isLast) const;

  IRI_STORAGE::IRIContext &ctx;
  std::vector<ClosureNode> nodes;
  std::vector<IRICFG *> preorder_;
  CIDX root_id = NO_CIDX;
  std::unordered_map<CIDX, IRI_STORAGE::IRID> canonical;
};

} // namespace IRI_STRUCTURAL
