#pragma once
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include <cstdint>
#include <string>
#include <vector>

namespace IRI_STRUCTURAL {

using namespace IRI_STORAGE;

//
// Finally moving to labeled edges, our interprocedural solver really needs this
// having hacky patches on top of each other was really slowing things down
// completely re-did this file, it was a compelte mess...
// The idea is simple, make CFG an immutable structure by default.
// Any structural change goes through a well defined class that asserts certain
// canonical rules while also replacing the old state with new one.
// ~Meetesh
//

//
// 1. Edges
//
enum class EdgeKind : uint8_t { Normal, Exception, Deferred };
using EdgeMask = uint8_t;
inline constexpr EdgeMask EDGE_NORMAL = 1u << (uint8_t)EdgeKind::Normal;
inline constexpr EdgeMask EDGE_EXCEPTION = 1u << (uint8_t)EdgeKind::Exception;
inline constexpr EdgeMask EDGE_DEFERRED = 1u << (uint8_t)EdgeKind::Deferred;
inline constexpr EdgeMask EDGE_ALL =
    EDGE_NORMAL | EDGE_EXCEPTION | EDGE_DEFERRED;
inline constexpr EdgeMask EDGE_FLOW = EDGE_NORMAL | EDGE_DEFERRED;
inline constexpr EdgeMask edgeMask(EdgeKind kind) {
  return 1u << (uint8_t)kind;
}
struct Edge {
  BBIDX to;
  EdgeKind kind;
};

//
// 2. Basic Blocks
//
struct IRIBB {
  SCPIDX SCOPE = NO_SCOPE;
  std::vector<IRID> stmts;
  IRID terminal = NO_IRID;
  std::vector<Edge> succs;
  std::vector<Edge> preds;
};

class BBEditor;

//
// 3. CFG
//
struct IRICFG {
  IRIContext &ctx;
  IRID container;
  BBIDX entry = NO_BB;
  BBIDX exit = NO_BB;
  IRICFG(IRID container, IRIContext &);
  void commit(const std::vector<BBIDX> &order);
  size_t numBBs() const { return blocks.size(); }
  const IRIBB &at(BBIDX h) const;
  BBIDX exceptionOf(BBIDX h) const;
  std::vector<BBIDX> reversePostOrder(EdgeMask mask = EDGE_ALL) const;
  std::vector<BBIDX> reversePostOrder(BBIDX start, EdgeMask mask) const;
  std::string debugID() const;
  std::string debugName(BBIDX h) const;
  std::string debugStmtID(BBIDX h, IRID stmt) const;

private:
  friend class BBEditor;
  std::vector<IRIBB> blocks;
};

//
// 4. Editor
//
class BBEditor {
public:
  BBEditor(IRICFG &cfg, BBIDX block);
  ~BBEditor();

  bool changed() const { return dirty; }
  void insertBefore(IRID anchor, IRID stmt);
  void insertAfter(IRID anchor, IRID stmt);
  void replace(IRID anchor, IRID stmt);
  void erase(IRID stmt);
  void setTerminal(IRID);
  void setException(BBIDX target);

private:
  size_t find(IRID anchor) const;
  void check(IRID stmt) const;

  IRICFG &cfg;
  BBIDX block;
  std::vector<IRID> body;
  bool dirty = false;
};

} // namespace IRI_STRUCTURAL
