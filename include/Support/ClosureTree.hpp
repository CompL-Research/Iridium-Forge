#pragma once

#include "IRICFG.hpp"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include <optional>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <vector>

namespace IRI_STRUCTURAL {

class ClosureTree {
private:
  // Node Storage Pool
  IRI_STORAGE::IridiumPool &pool;

  std::unordered_map<IRI_STORAGE::IRID, std::unique_ptr<IRICFG>> closures;
  std::unordered_map<IRI_STORAGE::IRID, std::vector<IRI_STORAGE::IRID>> outEdges;
  std::unordered_map<IRI_STORAGE::IRID, std::vector<IRI_STORAGE::IRID>> inEdges;
  std::unordered_map<double, IRICFG *> scopeIDXtoClosure;
  std::unordered_map<double, IRICFG *> bbIDXtoClosure;

  // Enforcing exactly one root
  std::optional<IRI_STORAGE::IRID> root_id;

  // Internal recursive helper for traversal
  void traverseRecursive(IRI_STORAGE::IRID current_id,
                          const std::function<void(IRICFG *)> &visitor) const;

  void dumpRecursive(std::ostream &os,
                      IRI_STORAGE::IRID current_id,
                      const std::string &prefix,
                      bool is_last) const;

public:
  ClosureTree(IRI_STORAGE::IridiumPool &, IRI_STORAGE::IRID);
  void addClosure(IRI_STORAGE::IRID id);
  void addEdge(IRI_STORAGE::IRID from, IRI_STORAGE::IRID to);
  void addEdgeFromScopeToBBIDX(double from, double to);

  IRICFG *getRoot();
  const IRICFG *getRoot() const;

  void preorderTraversal(const std::function<void(IRICFG *)> &visitor) const;

  void dumpFlat(std::ostream &oss) const;

  void commit();
};

} // namespace IRI_STRUCTURAL
