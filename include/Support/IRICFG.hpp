#pragma once
#include "Storage/BindingsPool.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include <unordered_map>
#include <vector>
#include <set>


namespace IRI_STRUCTURAL {

using namespace IRI_STORAGE;

using BBIDX = double;

class IRIBB;
class IRICFG;

// 1. Statements Inside the BB
struct IRIStatement {
  IRID id;
  IRIStatement *prev = nullptr;
  IRIStatement *next = nullptr;
  IRIBB *bb;
  explicit IRIStatement(IRID, IRIBB *);
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

  ~IRIBB();
  void append(IRIStatement *inst);
  explicit IRIBB(double, double, IRICFG *, IRI_STORAGE::IridiumPool &);

  void setTerminal(IRID);

  void dumpFlat(std::ostream &oss) const;
};

struct IRICFG {
  IRI_STORAGE::IridiumPool &pool;
  IRI_STORAGE::IRID id;
  IRI_STORAGE::IRID retCTX;

  // Directed edges representing control flow
  std::unordered_map<BBIDX, std::unique_ptr<IRIBB>> nodeMap;
  std::unordered_map<BBIDX, std::set<BBIDX>> successors;
  std::unordered_map<BBIDX, std::set<BBIDX>> predecessors;

  // Well-defined entry and exit points for the graph
  BBIDX entry_block;
  BBIDX exit_block;

  // Constructor declaration
  explicit IRICFG(IRI_STORAGE::IRID id, IRI_STORAGE::IridiumPool &);

  // Save the Closure's CFG as a DOT file at the given location
  void dumpDOT(const std::string &);

  // Reconstruct the BB SEXP
  void commit();

  // Compute Reverse Post-Order (RPO) of basic block indices
  std::vector<BBIDX> getReversePostOrder(bool includeExceptions = false) const;
};


} // namespace IRI_STRUCTURAL
