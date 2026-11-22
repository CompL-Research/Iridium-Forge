#pragma once
#include "Storage/BindingsPool.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include "external/Prakriti.hpp"
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace IRI_STRUCTURAL {

using namespace IRI_STORAGE;

using BBIDX = int;
using ScopeIDX = double;

// No block. EXCEPTION uses this instead of the old -1 sentinel.
inline constexpr BBIDX NO_BB = -1;

class IRIBB;
class IRICFG;

// Facts PTA derives from a closure's CFG and bindings.
struct PTAClosureInfo {
  std::vector<IRID> stack;      // uncaptured  -> StackObject
  std::vector<IRID> tstack;     // captured    -> TransientStackObject
  std::vector<IRID> remoteRefs; // captured from an enclosing scope
  std::vector<StringID> globals;
  std::set<double> usedJSCTXSlots;
  // Parameters, ordered by REFIDX so index i is the i'th actual argument.
  std::vector<std::pair<double, IRID>> formals;
};

// 1. Statements Inside the BB
struct IRIStatement {
  IRID id;
  IRIStatement *prev = nullptr;
  IRIStatement *next = nullptr;
  IRIBB *bb;
  explicit IRIStatement(IRID, IRIBB *);

  std::string getDebugID();

  void dumpFlat(std::ostream &oss, IRIContext *ctx, int depth = 0,
                bool full = true) const;
};

// 2. The Basic Block
struct IRIBB {
  BBIDX IDX;
  double SCOPE;
  BBIDX EXCEPTION = NO_BB;
  IRI_STORAGE::IRIContext &ctx;
  IRIStatement *head = nullptr;
  IRIStatement *tail = nullptr;
  IRICFG *closure;

  std::string getDebugID();
  std::string getDebugName();
  ~IRIBB();
  void append(IRIStatement *inst);
  void remove(IRIStatement *inst);
  void insertBefore(IRIStatement *inst, IRIStatement *before);
  void insertAfter(IRIStatement *inst, IRIStatement *after);
  void replace(IRIStatement *oldInst, IRIStatement *newInst);
  explicit IRIBB(double, BBIDX, IRICFG *, IRI_STORAGE::IRIContext &);

  void setTerminal(IRID);

  void clearEdges();
  void addEdge(BBIDX succ);
  void moveTerminalTo(IRIBB *dst);

  void dumpFlat(std::ostream &oss) const;

  std::string getInstID(IRIStatement *r) {
    size_t i = 0;

    for (IRIStatement *s = head; s != nullptr; s = s->next) {
      if (s == r)
        return std::to_string(i);
      i++;
    }
    if (tail == r)
      return std::to_string(i);
    throw std::runtime_error("Failed to find INST in BB, unexpected...");
  }
};

struct IRICFG {
  IRI_STORAGE::IRIContext &ctx;
  IRI_STORAGE::IRID id;
  IRI_STORAGE::IRID retCTX;

  // During construction blocks arrive in the program-wide numbering, so the
  // node set and edge sets are sparse maps. renumberBBs() collapses them into
  // the dense CFG-local representation below; after the constructor finishes,
  // only these exist and every handle is a dense index.
  std::unordered_map<BBIDX, std::unique_ptr<IRIBB>> sparseNodes;
  std::unordered_map<BBIDX, std::set<BBIDX>> sparseSuccs;
  std::unordered_map<BBIDX, std::set<BBIDX>> sparsePreds;

  std::vector<std::unique_ptr<IRIBB>> bbs;               // by local handle
  std::vector<std::set<BBIDX>> successors, predecessors; // indexed by handle

  IRIBB *makeBB(ScopeIDX scope);
  void verify();

  // IRIBB::addEdge/clearEdges route through insertEdge/removeEdge, which
  // reach into the edge containers directly.
  friend struct IRIBB;

private:
  void removeUnreachable();
  void renumberBBs();
  void insertEdge(BBIDX from, BBIDX to);
  void removeEdge(BBIDX from, BBIDX to);
  void eraseSparseNode(BBIDX idx);

public:
  size_t numBBs() const { return bbs.size(); }

  IRIBB *get_bb(BBIDX idx) {
    if (idx < 0 || (size_t)idx >= bbs.size() || !bbs[idx])
      throw std::runtime_error("IRICFG could not find the BB");
    return bbs[idx].get();
  }

  const IRIBB *get_bb(BBIDX idx) const {
    if (idx < 0 || (size_t)idx >= bbs.size() || !bbs[idx])
      throw std::runtime_error("IRICFG could not find the BB");
    return bbs[idx].get();
  }

  // Successor/predecessor sets of a block. Out-of-range handles read as the
  // empty set; mutation goes through addEdge/clearEdges.
  const std::set<BBIDX> &successorsOf(BBIDX idx) const {
    if (idx < 0 || (size_t)idx >= successors.size())
      return emptySuccs;
    return successors[idx];
  }
  const std::set<BBIDX> &predecessorsOf(BBIDX idx) const {
    if (idx < 0 || (size_t)idx >= predecessors.size())
      return emptySuccs;
    return predecessors[idx];
  }

  // The entry's id in the program-wide numbering, kept so the closure tree
  // and core-IR statements can still name this CFG's entry. The CFG itself
  // only ever uses dense local handles.
  BBIDX globalEntryID = NO_BB;

  // local handle -> the global id this block had in the core IR (identity
  // before renumbering). commit() translates back with this.
  BBIDX globalBBID(BBIDX local) const {
    if (local < 0 || (size_t)local >= globalIDs.size())
      throw std::runtime_error("globalBBID: handle out of range");
    return globalIDs[local];
  }

  // Well-defined entry and exit points for the graph
  BBIDX entry_block;
  BBIDX exit_block;

  // What PTA needs from this closure, derived in one pass and cached. Any
  // structural change drops it via markDirty(); the next query rebuilds it.
  const PTAClosureInfo &ptaInfo();
  void markDirty() { ptaInfoCache.reset(); }
  std::optional<PTAClosureInfo> ptaInfoCache;

  // Constructor declaration
  explicit IRICFG(IRI_STORAGE::IRID id, IRI_STORAGE::IRIContext &);

  // Save the Closure's CFG as a DOT file at the given location
  void dumpDOT(const std::string &);

  std::string getDebugID();
  std::string getDebugName();

  // Reconstruct the BB SEXP
  void commit();

  // Compute Reverse Post-Order (RPO) of basic block indices
  std::vector<BBIDX> getReversePostOrder(bool includeExceptions,
                                         BBIDX startBBIDX) const;
  std::vector<BBIDX> getReversePostOrder(bool includeExceptions = false) const;

private:
  std::set<BBIDX> continuationPoints;
  std::vector<BBIDX> globalIDs; // local -> global (identity before renumber)
  inline static const std::set<BBIDX> emptySuccs{};
};

} // namespace IRI_STRUCTURAL

// Readability shorthand for IRICFG::get_bb, replacing the old (*cfg)[idx]
// operator[] syntax.
#define IRI_BB(cfg, idx) ((cfg)->get_bb(idx))
