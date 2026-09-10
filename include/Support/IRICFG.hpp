#pragma once
#include "Storage/BindingsPool.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "external/Prakriti.hpp"
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

// 1. Statements Inside the BB
struct IRIStatement {
  IRID id;
  IRIStatement *prev = nullptr;
  IRIStatement *next = nullptr;
  IRIBB *bb;
  explicit IRIStatement(IRID, IRIBB *);

  std::string getDebugID();
};

// 2. The Basic Block
struct IRIBB {
  double IDX;
  double SCOPE;
  double EXCEPTION = -1;
  double FINTARGET = -1;
  IRI_STORAGE::IridiumPool &pool;
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
  explicit IRIBB(double, double, IRICFG *, IRI_STORAGE::IridiumPool &);

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
  IRI_STORAGE::IridiumPool &pool;
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

  IRIBB *operator[](BBIDX idx) {
    auto it = nodeMap.find(idx);
    if (it == nodeMap.end())
      throw std::runtime_error("IRICFG could not find the BB");
    else
      return nodeMap[idx].get();
  }

  // Well-defined entry and exit points for the graph
  BBIDX entry_block;
  BBIDX exit_block;

  // PTA Support methods
  //
  // 1. getUncapturedStackBindings : StackNode
  // 2. getCapturedStackBindings   : TransientStackNode
  // 3. getRemoteReferences        : ASSERT exists
  //    (i)  RemoteEnvBinding :: TransientStackNodes
  //    (ii) GlobalBinding    :: PKRGlobalState Binding
  //
  std::vector<IRID> ptaGenStack();
  std::vector<IRID> ptaGenTStack();
  std::vector<IRID> ptaAssertTransient();
  std::vector<StringID> ptaAssertGlobals();

  // Constructor declaration
  explicit IRICFG(IRI_STORAGE::IRID id, IRI_STORAGE::IridiumPool &);

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
