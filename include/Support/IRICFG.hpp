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

using BBIDX = double;
using ScopeIDX = double;

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
  double IDX;
  double SCOPE;
  double EXCEPTION = -1;
  double FINTARGET = -1;
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
  explicit IRIBB(double, double, IRICFG *, IRI_STORAGE::IRIContext &);

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

  // Directed edges representing control flow
  std::unordered_map<BBIDX, std::unique_ptr<IRIBB>> nodeMap;
  std::unordered_map<BBIDX, std::set<BBIDX>> successors;
  std::unordered_map<BBIDX, std::set<BBIDX>> predecessors;
  std::set<BBIDX> continuationPoints;

  IRIBB *makeBB(ScopeIDX scope);
  BBIDX finalizerRetOf(BBIDX entryIDX);
  void retagFinalizerRet(BBIDX from, BBIDX to);
  void verify();
  std::map<BBIDX, BBIDX> finalizerRetBB;

  IRIBB *get_bb(BBIDX idx) {
    auto it = nodeMap.find(idx);
    if (it == nodeMap.end())
      throw std::runtime_error("IRICFG could not find the BB");
    else
      return nodeMap[idx].get();
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
                                         double startBBIDX) const;
  std::vector<BBIDX> getReversePostOrder(bool includeExceptions = false) const;
};

} // namespace IRI_STRUCTURAL

// Readability shorthand for IRICFG::get_bb, replacing the old (*cfg)[idx]
// operator[] syntax.
#define IRI_BB(cfg, idx) ((cfg)->get_bb(idx))
