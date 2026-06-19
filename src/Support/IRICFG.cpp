#include "Support/IRICFG.hpp"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Storage/Config.h"
#include "Storage/IridiumPool.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/IRIS.hpp"
#include <cassert> // For assert()
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept> // For std::runtime_error
#include <vector>

namespace IRI_STRUCTURAL {
using namespace IRI_STORAGE;
using namespace IRI_GEN;

IRIStatement::IRIStatement(IRID id, IRIBB *b) : id(id), bb(b) {}

IRIBB::IRIBB(double ss, double i, IRICFG *c, IRI_STORAGE::IridiumPool &p)
    : SCOPE(ss), IDX(i), closure(c), pool(p) {}

IRIBB::~IRIBB() {
  IRIStatement *curr = head;
  while (curr != nullptr) {
    auto & next = curr->next;
    delete curr;
    curr = next;
  }
  if (tail != nullptr) {
    delete tail;
  }
}

void IRIBB::append(IRIStatement *inst) {
  if (head == nullptr) {
    head = inst;
    inst->prev = nullptr;
    inst->next = nullptr;
    return;
  }
  IRIStatement *last = head;
  while (last->next != nullptr) {
    last = last->next;
  }
  last->next = inst;
  inst->next = nullptr;
  inst->prev = last;
}

static bool isValidBBTerminal(IRI_GEN::IRI_TAG tag, IridiumPool &pool) {
  if (ReturnAsync == tag)
    return true;
  if (Goto == tag)
    return true;
  if (IfElseJump == tag)
    return true;
  if (Ret == tag)
    return true;
  if (Return == tag)
    return true;
  if (Throw == tag)
    return true;
  if (InvokeFinalizer == tag)
    return true;
  return false;
}

void IRIBB::setTerminal(IRID stmtID) {
  if (tail != nullptr) {
    // Clear existing edges
    for (auto & e : closure->successors[IDX]) {
      closure->predecessors[e].erase(IDX);
    }
    closure->successors[IDX].clear();
    // Updating a terminal deletes the existing
    delete tail;
  }

  IRI_TAG stmtTAG = pool[stmtID].tag;
  assert(isValidBBTerminal(stmtTAG, pool));
  switch (stmtTAG) {
  case IRI_GEN::Return:
  case IRI_GEN::ReturnAsync: {
    IRID rValTarget;
    if (stmtTAG == Return) {
      ReturnSEXP r(stmtID, pool);
      rValTarget = r.getArg_Obj();
    } else {
      ReturnAsyncSEXP rAsync(stmtID, pool);
      rValTarget = rAsync.getArg_RetVal();
    }

    append(new IRIStatement(LWriteSEXP::create(pool, closure->retCTX,
                                               rValTarget, false, true, false),
                            this));

    tail = new IRIStatement(GotoSEXP::create(pool, closure->exit_block), this);

    closure->successors[IDX].insert(closure->exit_block);
    closure->predecessors[closure->exit_block].insert(IDX);
    break;
  }
  case IRI_GEN::Goto: {
    tail = new IRIStatement(stmtID, this);
    GotoSEXP gotoSEXP(stmtID, pool);
    closure->successors[IDX].insert(gotoSEXP.getIDX());
    closure->predecessors[gotoSEXP.getIDX()].insert(IDX);
    break;
  }
  case IRI_GEN::IfElseJump: {
    tail = new IRIStatement(stmtID, this);
    IfElseJumpSEXP ieJump(stmtID, pool);
    closure->successors[IDX].insert(ieJump.getTRUE());
    closure->predecessors[ieJump.getTRUE()].insert(IDX);

    closure->successors[IDX].insert(ieJump.getFALSE());
    closure->predecessors[ieJump.getFALSE()].insert(IDX);
    break;
  }
  case IRI_GEN::Ret: {
    tail = new IRIStatement(stmtID, this);
    break;
  }
  case IRI_GEN::Throw: {
    tail = new IRIStatement(stmtID, this);
    break;
  }
  case IRI_GEN::InvokeFinalizer: {
    tail = new IRIStatement(stmtID, this);
    break;
  }
  default:
    throw std::runtime_error("Unexpected case for terminal stmt");
  }
}

static std::string escapeDoubleQuotes(const std::string &input) {
  std::stringstream ss;

  for (char c : input) {
    if (c == '"') {
      ss << "\\\""; // Prepends the backslash to the quote
    } else {
      ss << c;
    }
  }

  return ss.str();
}

void IRIBB::dumpFlat(std::ostream &oss) const {
  IRIStatement *curr = head;
  while (curr != nullptr) {
    std::stringstream ss;
    pool[curr->id].dumpFlat(ss, &pool, 0, false);
    oss << escapeDoubleQuotes(ss.str()) << "\\l";
    curr = curr->next;
  }

  if (tail) {
    std::stringstream ss;
    pool[tail->id].dumpFlat(ss, &pool, 0, false);
    oss << escapeDoubleQuotes(ss.str());
  }
}

IRICFG::IRICFG(IRI_STORAGE::IRID i, IRI_STORAGE::IridiumPool &p)
    : id(i), pool(p) {
  BBContainerSupport bbc(id, pool);
  // Entry BBIDX
  bool wasEntrySet = false;
  entry_block = bbc.getStartBBIDX();

  // Common Return Path
  IRI_STORAGE::StringID commonRet = pool.strings.intern("<common-ret>");
  retCTX = pool.iris
               ->declareLBinding(bbc.getScopeIDX(), commonRet,
                                 IRI_GEN::IRI_FLAG::JSVAR)
               .ID;
  exit_block = ++pool.lastBBIDX;
  nodeMap[exit_block] =
      std::make_unique<IRIBB>(bbc.getScopeIDX(), exit_block, this, pool);

  if (bbc.hasASYNC() || bbc.hasGENERATOR()) {
    IRIStatement *retStmt = new IRIStatement(
        ReturnAsyncSEXP::create(pool,
                                EnvReadSEXP::create(pool, retCTX, true, false)),
        nodeMap[exit_block].get());
    nodeMap[exit_block]->tail = retStmt;
  } else {
    IRIStatement *retStmt = new IRIStatement(
        ReturnSEXP::create(pool,
                           EnvReadSEXP::create(pool, retCTX, true, false)),
        nodeMap[exit_block].get());
    nodeMap[exit_block]->tail = retStmt;
  }

  // Add all the basic blocks to the graph
  for (auto [bbID, bbOffset] : bbc.bbs()) {
    BBSupport bb(bbID, pool);
    auto currBBIDX = bb.getIDX();

    nodeMap[currBBIDX] =
        std::make_unique<IRIBB>(bb.getScopeIDX(), currBBIDX, this, pool);
    if (currBBIDX == entry_block) {
      assert(!wasEntrySet);
      wasEntrySet = true;
    }
  }

  assert(wasEntrySet);

  // Populate statements and establish edges
  for (auto [bbID, bbOffset] : bbc.bbs()) {
    BBSupport bbSupp(bbID, pool);
    auto currBBIDX = bbSupp.getIDX();
    IRIBB *bb = nodeMap[currBBIDX].get();
    bb->EXCEPTION = pool.iris->getExceptionTargetForScope(bb->SCOPE);
    auto stmts = bbSupp.stmtsVec();
    for (size_t i = 0; i < stmts.size(); i++) {
      IRID stmtID = stmts[i];
      IRI_TAG stmtTAG = pool[stmtID].tag;
      if (i + 1 == stmts.size()) {
        bb->setTerminal(stmtID);
      } else {
        bb->append(new IRIStatement(stmtID, bb));
      }
    }
  }

  // Split BBs at Finalizer Calls
  std::set<IRIBB *> worklist;
  for (auto & e : nodeMap) {
    worklist.insert(e.second.get());
  }
  while (!worklist.empty()) {
    IRIBB *bb = *worklist.begin();
    worklist.erase(worklist.begin());

    IRIStatement *curr = bb->head;
    while (curr != nullptr) {
      if (pool[curr->id].tag == IRI_GEN::InvokeFinalizer) {
        break;
      }
      curr = curr->next;
    }

    if (curr != nullptr) {
      InvokeFinalizerSEXP ivTarget(curr->id, pool);
      double finalizerEntryIDX = ivTarget.getIDX();
      double finalizerExitIDX = pool.iris->getFinalizerRetBBIDX(finalizerEntryIDX);
      if (!nodeMap.contains(finalizerEntryIDX)) {
        throw std::runtime_error("finalizerEntryIDX missing");
      }

      if (!nodeMap.contains(finalizerExitIDX)) {
        throw std::runtime_error("finalizerExitIDX missing");
      }

      // Prep PostBB
      auto postBBIDX = ++pool.lastBBIDX;
      nodeMap[postBBIDX] =
          std::make_unique<IRIBB>(bb->SCOPE, postBBIDX, this, pool);
      IRIBB *postBB = nodeMap[postBBIDX].get();
      worklist.insert(postBB);
      postBB->head = curr->next;
      postBB->tail = new IRIStatement(bb->tail->id, bb);

      // Forward successors of the currentBB to PostBB
      for (auto & s : successors[bb->IDX]) {
        successors[postBBIDX].insert(s);
        predecessors[s].insert(postBBIDX);
      }

      bool finalizerDynamicRet = pool[nodeMap[finalizerExitIDX]->tail->id].tag == Ret;

      // Create Edge from finalizer BB Exit to PostBB
      if (finalizerDynamicRet) {
        successors[finalizerExitIDX].insert(postBBIDX);
        predecessors[postBBIDX].insert(finalizerExitIDX);
      }

      // Make the prev stmt the last
      if (curr->prev) {
        curr->prev->next = nullptr;
      } else {
        bb->head = nullptr;
        assert(bb->head == curr);
      }

      // Set the invoke finalizer as the terminal
      bb->setTerminal(curr->id);
      if (finalizerDynamicRet) {
        bb->FINTARGET = postBBIDX;
      } else {
        bb->FINTARGET = -2;
      }
      delete curr;
      successors[bb->IDX].insert(finalizerEntryIDX);
      predecessors[finalizerEntryIDX].insert(bb->IDX);
    }
  }
}

void IRICFG::dumpDOT(const std::string &filePath) {
  std::ofstream outFile(filePath);
  assert(outFile.is_open() &&
         "Failed to open file: Path may be invalid or restricted.");
  if (!outFile) {
    throw std::runtime_error("Error: Could not open file at path: " + filePath);
  }

  outFile << "digraph G {\n";
  outFile << "  graph [\n";
  outFile << "    rankdir=TB\n";
  outFile << "    splines=true\n";
  outFile << "    overlap=false\n";
  outFile << "  ]\n";

  // Strict Monospace Typography and Left Alignment
  outFile << "  node [\n";
  outFile << "    shape=box\n";
  outFile << "    style=\"filled,rounded\"\n";
  outFile << "    fillcolor=\"#f6f8fa\"\n";
  outFile << "    color=\"#d0d7de\"\n";
  outFile << "    fontname=\"monospace\"\n";
  outFile << "    fontsize=12\n";
  outFile << "    labelloc=\"c\"\n";
  outFile << "    labeljust=\"l\"\n";
  outFile << "    margin=\"0.2,0.1\"\n";
  outFile << "  ]\n";

  std::set<BBIDX> nodesTODO;
  for (auto &e : nodeMap) {
    nodesTODO.insert(e.first);
  }

  std::function<void(BBIDX)> dumpRecurse = [&](BBIDX currentBBIDX) {
    if (!nodesTODO.contains(currentBBIDX))
      return;
    nodesTODO.erase(currentBBIDX);

    std::stringstream ss;
    auto &bb = nodeMap[currentBBIDX];
    bb->dumpFlat(ss);
    outFile << "  " << (int)currentBBIDX << " [label=\"" << ss.str()
            << "\", xlabel=\"BB" << currentBBIDX << "\"];\n";

    for (auto succ : successors[currentBBIDX]) {
      outFile << "  " << (int)currentBBIDX << " -> " << (int)succ << ";\n";
      dumpRecurse(succ);
    }

    if (bb->EXCEPTION > -1) {
      outFile << "  " << (int)currentBBIDX << " -> " << (int)bb->EXCEPTION << " [style=\"dotted\", color=\"red\"];\n";
      dumpRecurse(bb->EXCEPTION);
    }

    if (bb->FINTARGET > -1) {
      outFile << "  " << (int)currentBBIDX << " -> " << (int)bb->FINTARGET << " [style=\"dotted\"];\n";
    }
  };
  dumpRecurse(entry_block);

  for (auto & wasteBB : nodesTODO) {
    std::cerr << "  DeadBB: " << wasteBB << std::endl;
  }

  outFile << "}\n";
}

void IRICFG::commit() {
  std::vector<IRID> newBBS;
  BBContainerSupport bbc(id, pool);
  IRID newBBList = ListSEXP::create(pool, pool.strings.intern("BB"));
  bbc.setArg_BB(newBBList);

  std::set<BBIDX> nodesTODO;
  for (auto &e : nodeMap) {
    nodesTODO.insert(e.first);
  }

  std::function<void(BBIDX)> dumpRecurse = [&](BBIDX currentBBIDX) {
    if (!nodesTODO.contains(currentBBIDX))
      return;
    nodesTODO.erase(currentBBIDX);

    auto &bb = nodeMap[currentBBIDX];
    IRID currBBID = BBSEXP::create(pool, false, false, false, false, false,
                                   bb->IDX, bb->SCOPE);
    newBBS.push_back(currBBID);

    std::vector<IRID> stmtList;

    IRIStatement *curr = bb->head;
    while (curr != nullptr) {
      stmtList.push_back(curr->id);
      curr = curr->next;
    }
    if (bb->tail != nullptr) {
      stmtList.push_back(bb->tail->id);
    }
    if (bb->FINTARGET == -2) {
      stmtList.push_back(ThrowSEXP::create(pool, StringSEXP::create(pool, pool.strings.intern("Unreachable"))));
    } else if (bb->FINTARGET > -1) {
      stmtList.push_back(GotoSEXP::create(pool, bb->FINTARGET));
    }

    pool.set_args(currBBID, stmtList);

    for (auto succ : successors[currentBBIDX]) {
      dumpRecurse(succ);
    }

    if (bb->EXCEPTION > -1) {
      dumpRecurse(bb->EXCEPTION);
    }
  };
  dumpRecurse(entry_block);

  pool.set_args(newBBList, newBBS);
}
} // namespace IRI_STRUCTURAL
