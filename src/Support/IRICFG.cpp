#include "Support/IRICFG.hpp"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Storage/Config.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/IRIS.hpp"
#include <algorithm>
#include <cassert> // For assert()
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept> // For std::runtime_error
#include <string>
#include <vector>

namespace IRI_STRUCTURAL {
using namespace IRI_STORAGE;
using namespace IRI_GEN;

IRIStatement::IRIStatement(IRID id, IRIBB *b) : id(id), bb(b) {}

IRIBB::IRIBB(double ss, BBIDX i, IRICFG *c, IRI_STORAGE::IRIContext &p)
    : SCOPE(ss), IDX(i), closure(c), ctx(p) {}

IRIBB::~IRIBB() {
  IRIStatement *curr = head;
  while (curr != nullptr) {
    IRIStatement *next = curr->next;
    delete curr;
    curr = next;
  }
  if (tail != nullptr) {
    delete tail;
  }
}

void IRIBB::append(IRIStatement *inst) {
  closure->markDirty();
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

void IRIBB::remove(IRIStatement *inst) {
  closure->markDirty();
  if (inst == nullptr)
    return;
  if (inst == tail) {
    tail = nullptr;
    delete inst;
    return;
  }
  if (inst->prev != nullptr) {
    inst->prev->next = inst->next;
  } else {
    head = inst->next;
  }
  if (inst->next != nullptr) {
    inst->next->prev = inst->prev;
  }
  delete inst;
}

void IRIBB::insertBefore(IRIStatement *inst, IRIStatement *before) {
  closure->markDirty();
  if (inst == nullptr)
    return;
  if (before == nullptr) {
    append(inst);
    return;
  }
  inst->next = before;
  inst->prev = before->prev;
  if (before->prev != nullptr) {
    before->prev->next = inst;
  } else {
    head = inst;
  }
  before->prev = inst;
}

void IRIBB::insertAfter(IRIStatement *inst, IRIStatement *after) {
  closure->markDirty();
  if (inst == nullptr)
    return;
  if (after == nullptr) {
    inst->next = head;
    inst->prev = nullptr;
    if (head != nullptr) {
      head->prev = inst;
    }
    head = inst;
    return;
  }
  inst->prev = after;
  inst->next = after->next;
  if (after->next != nullptr) {
    after->next->prev = inst;
  }
  after->next = inst;
}

void IRIBB::replace(IRIStatement *oldInst, IRIStatement *newInst) {
  closure->markDirty();
  if (oldInst == nullptr || newInst == nullptr)
    return;
  if (oldInst == tail) {
    tail = newInst;
    newInst->prev = nullptr;
    newInst->next = nullptr;
    delete oldInst;
    return;
  }
  newInst->prev = oldInst->prev;
  newInst->next = oldInst->next;
  if (oldInst->prev != nullptr) {
    oldInst->prev->next = newInst;
  } else {
    head = newInst;
  }
  if (oldInst->next != nullptr) {
    oldInst->next->prev = newInst;
  }
  delete oldInst;
}

static bool isValidBBTerminal(IRI_GEN::IRI_TAG tag, IRIContext &ctx) {
  if (ReturnAsync == tag)
    return true;
  if (Goto == tag)
    return true;
  if (IfElseJump == tag)
    return true;
  if (Return == tag)
    return true;
  if (Throw == tag)
    return true;
  return false;
}

void IRIBB::clearEdges() {
  closure->markDirty();
  // Copy first: removeEdge mutates the set being walked.
  auto succs = (IDX < (BBIDX)closure->successors.size())
                   ? closure->successors[IDX]
                   : (closure->sparseSuccs.contains(IDX)
                          ? closure->sparseSuccs[IDX]
                          : std::set<BBIDX>{});
  for (auto s : succs) {
    closure->removeEdge(IDX, s);
  }
}

void IRIBB::addEdge(BBIDX succ) {
  closure->markDirty();
  closure->insertEdge(IDX, succ);
}

void IRICFG::insertEdge(BBIDX from, BBIDX to) {
  markDirty();
  if (from < 0 || (size_t)from >= successors.size()) {
    sparseSuccs[from].insert(to);
    sparsePreds[to].insert(from);
  } else {
    successors[from].insert(to);
    predecessors[to].insert(from);
  }
}

void IRICFG::removeEdge(BBIDX from, BBIDX to) {
  markDirty();
  if (from < 0 || (size_t)from >= successors.size()) {
    sparseSuccs[from].erase(to);
    sparsePreds[to].erase(from);
  } else {
    successors[from].erase(to);
    predecessors[to].erase(from);
  }
}

void IRIBB::moveTerminalTo(IRIBB *dst) {
  closure->markDirty();
  if (tail == nullptr) {
    throw std::runtime_error("moveTerminalTo: source has no terminal");
  }
  if (dst->tail != nullptr) {
    throw std::runtime_error("moveTerminalTo: dst already has a terminal");
  }

  dst->tail = tail;
  dst->tail->bb = dst;
  dst->tail->prev = nullptr;
  dst->tail->next = nullptr;
  tail = nullptr;

  auto succs = closure->successorsOf(IDX);
  if (IDX >= (BBIDX)closure->successors.size() &&
      closure->sparseSuccs.contains(IDX))
    succs = closure->sparseSuccs.at(IDX);
  clearEdges();
  for (auto s : succs) {
    dst->addEdge(s);
  }
}

IRIBB *IRICFG::makeBB(ScopeIDX scope) {
  markDirty();
  auto idx = (BBIDX)(++ctx.lastBBIDX);
  sparseNodes[idx] = std::make_unique<IRIBB>(scope, idx, this, ctx);
  return sparseNodes[idx].get();
}

void IRIBB::setTerminal(IRID stmtID) {
  closure->markDirty();
  if (tail != nullptr) {
    clearEdges();
    delete tail;
    tail = nullptr;
  }

  IRI_TAG stmtTAG = IRI_NODE(ctx, stmtID).tag;
  assert(isValidBBTerminal(stmtTAG, ctx));
  switch (stmtTAG) {
  case IRI_GEN::Return:
  case IRI_GEN::ReturnAsync: {
    IRID rValTarget;
    if (stmtTAG == Return) {
      ReturnSEXP r(stmtID, ctx);
      rValTarget = r.getArg_Obj();
    } else {
      ReturnAsyncSEXP rAsync(stmtID, ctx);
      rValTarget = rAsync.getArg_RetVal();
    }

    append(new IRIStatement(LWriteSEXP::create(ctx, closure->retCTX, rValTarget,
                                               false, true, false),
                            this));

    tail = new IRIStatement(GotoSEXP::create(ctx, false, closure->exit_block),
                            this);
    addEdge(closure->exit_block);
    break;
  }
  case IRI_GEN::Goto: {
    tail = new IRIStatement(stmtID, this);
    GotoSEXP gotoSEXP(stmtID, ctx);
    addEdge(gotoSEXP.getIDX());
    break;
  }
  case IRI_GEN::IfElseJump: {
    tail = new IRIStatement(stmtID, this);
    IfElseJumpSEXP ieJump(stmtID, ctx);
    addEdge(ieJump.getTRUE());
    addEdge(ieJump.getFALSE());
    break;
  }
  case IRI_GEN::Throw: {
    tail = new IRIStatement(stmtID, this);
    // Force CFGs to have a single entry and well defined exit bb.
    addEdge(closure->exit_block);
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

void IRIStatement::dumpFlat(std::ostream &oss, IRIContext *ctx, int depth,
                            bool full) const {
  IRI_NODE(bb->ctx, id).dumpFlat(oss, &bb->ctx, depth, full);
}

void IRIBB::dumpFlat(std::ostream &oss) const {
  IRIStatement *curr = head;
  while (curr != nullptr) {
    std::stringstream ss;
    IRI_NODE(ctx, curr->id).dumpFlat(ss, &ctx, 0, false);
    oss << escapeDoubleQuotes(ss.str()) << "\\l";
    curr = curr->next;
  }

  if (tail) {
    std::stringstream ss;
    IRI_NODE(ctx, tail->id).dumpFlat(ss, &ctx, 0, false);
    oss << escapeDoubleQuotes(ss.str());
  }
}

void IRICFG::verify() {
  // EXCEPTION dispatch targets carry their incoming edge in the EXCEPTION
  // field, not in the predecessors map.
  std::set<BBIDX> exceptionTargets;
  for (auto &bb : bbs) {
    if (bb->EXCEPTION != NO_BB)
      exceptionTargets.insert(bb->EXCEPTION);
  }

  for (auto &bbp : bbs) {
    IRIBB *bb = bbp.get();

    if (bb->tail == nullptr) {
      throw std::runtime_error("BB has no terminal");
    }

    IRIStatement *curr = bb->head;
    while (curr != nullptr) {
      IRI_GEN::IRI_META currMeta =
          IRI_GEN::get_meta(IRI_NODE(ctx, curr->id).tag);
      if (currMeta != IRI_GEN::IRI_META::STMT &&
          currMeta != IRI_GEN::IRI_META::AMP) {
        std::cerr << IRI_GEN::dump_tag(IRI_NODE(ctx, curr->id).tag)
                  << " expected STMT, found: " << std::to_string(currMeta)
                  << std::endl;
        throw std::runtime_error("STMT err");
      }
      if (curr->bb != bb) {
        throw std::runtime_error("statement bb back-pointer is stale");
      }
      // Exception target and PushCatchContext must alwayas be the same in a BB,
      // right?
      if (IRI_NODE(ctx, curr->id).tag == IRI_GEN::PushCatchContext &&
          (double)bb->EXCEPTION !=
              PushCatchContextSEXP(curr->id, ctx).getIDX()) {
        throw std::runtime_error("PushCatchContext target diverges from "
                                 "EXCEPTION in BB " +
                                 std::to_string((int)bb->IDX));
      }
      // A block's control flow is set exclusively by its terminal statement
      // (modulo possibe exception edge)
      if (curr != bb->tail) {
        switch (IRI_NODE(ctx, curr->id).tag) {
        case IRI_GEN::Goto:
        case IRI_GEN::IfElseJump:
        case IRI_GEN::Throw:
        case IRI_GEN::Return:
        case IRI_GEN::ReturnAsync:
        case IRI_GEN::IfJump:
          throw std::runtime_error(
              std::string("control-flow statement mid-block in BB ") +
              std::to_string((int)bb->IDX));
        default:
          break;
        }
      }
      curr = curr->next;
    }
    if (bb->tail->bb != bb) {
      throw std::runtime_error("terminal bb back-pointer is stale");
    }

    auto &succSet = successors[bb->IDX];
    for (auto s : succSet) {
      if (s < 0 || (size_t)s >= bbs.size() || !bbs[s]) {
        throw std::runtime_error("successor missing from CFG");
      }
      if (!predecessors[s].contains(bb->IDX)) {
        throw std::runtime_error("successor/predecessor asymmetry");
      }
    }
    if (bb->EXCEPTION != NO_BB) {
      if (bb->EXCEPTION < 0 || (size_t)bb->EXCEPTION >= bbs.size() ||
          !bbs[bb->EXCEPTION]) {
        throw std::runtime_error("EXCEPTION target missing from CFG");
      }
      if (bb->EXCEPTION == bb->IDX) {
        throw std::runtime_error("EXCEPTION edge to self");
      }
      if (bb->IDX == exit_block) {
        throw std::runtime_error("EXIT has an EXCEPTION edge");
      }
    }

    // Terminal tag fixes the successor count.
    size_t expectedSuccs = 2;
    switch (IRI_NODE(ctx, bb->tail->id).tag) {
    case IRI_GEN::Goto:
    case IRI_GEN::Throw:
      expectedSuccs = 1;
      break;
    case IRI_GEN::IfElseJump:
      expectedSuccs = 2;
      break;
    case IRI_GEN::Return:
    case IRI_GEN::ReturnAsync:
      expectedSuccs = 0;
      break;
    default:
      throw std::runtime_error("BB terminal is not a valid terminator");
    }
    if (successors[bb->IDX].size() != expectedSuccs) {
      throw std::runtime_error("terminal/successor arity mismatch in BB " +
                               std::to_string((int)bb->IDX));
    }
    if (IRI_NODE(ctx, bb->tail->id).tag == IRI_GEN::Throw &&
        *successors[bb->IDX].begin() != exit_block) {
      throw std::runtime_error("Throw terminal must target EXIT in BB " +
                               std::to_string((int)bb->IDX));
    }
    // Also verify the targets of terminals match the CFGs succ/pred map
    {
      auto &succs = successors[bb->IDX];
      auto termTag = IRI_NODE(ctx, bb->tail->id).tag;
      if (termTag == IRI_GEN::Goto &&
          !succs.contains((BBIDX)GotoSEXP(bb->tail->id, ctx).getIDX())) {
        throw std::runtime_error("Goto target diverges from successors in BB " +
                                 std::to_string((int)bb->IDX));
      }
      if (termTag == IRI_GEN::IfElseJump) {
        auto tgt = IfElseJumpSEXP(bb->tail->id, ctx);
        if (!succs.contains((BBIDX)tgt.getTRUE()) ||
            !succs.contains((BBIDX)tgt.getFALSE())) {
          throw std::runtime_error("IfElseJump targets diverge from "
                                   "successors in BB " +
                                   std::to_string((int)bb->IDX));
        }
      }
    }
    if (expectedSuccs == 0 && bb->IDX != exit_block) {
      throw std::runtime_error(
          "Return/ReturnAsync terminal outside EXIT in BB " +
          std::to_string((int)bb->IDX));
    }

    auto &preds = predecessors[bb->IDX];
    auto &succs = successors[bb->IDX];
    if (bb->IDX == entry_block && !preds.empty()) {
      throw std::runtime_error("ENTRY has predecessors");
    }
    if (bb->IDX == exit_block && !succs.empty()) {
      throw std::runtime_error("EXIT has successors");
    }
    // Handler dispatch targets are entered through the EXCEPTION edge, so a
    // BB needs an incoming normal pred, an incoming EXCEPTION edge, or to be
    // ENTRY.
    if (bb->IDX != entry_block && bb->IDX != exit_block && preds.empty() &&
        !exceptionTargets.contains(bb->IDX)) {
      throw std::runtime_error("BB with no incoming edges: " +
                               std::to_string((int)bb->IDX));
    }
    if (bb->IDX != exit_block && succs.empty()) {
      throw std::runtime_error("BB with no successors: " +
                               std::to_string((int)bb->IDX));
    }
  }

  // Full reachability from ENTRY over normal + EXCEPTION edges.
  std::set<BBIDX> fwd;
  std::function<void(BBIDX)> fwdDFS = [&](BBIDX u) {
    if (!fwd.insert(u).second)
      return;
    for (auto s : successorsOf(u)) {
      fwdDFS(s);
    }
    if (u >= 0 && (size_t)u < bbs.size() && bbs[u]) {
      BBIDX exc = bbs[u]->EXCEPTION;
      if (exc != NO_BB)
        fwdDFS(exc);
    }
  };
  fwdDFS(entry_block);
  if (fwd.size() != bbs.size()) {
    throw std::runtime_error("CFG has unreachable BBs");
  }
}

void IRICFG::eraseSparseNode(BBIDX idx) {
  for (auto s : sparseSuccs[idx]) {
    sparsePreds[s].erase(idx);
  }
  for (auto p : sparsePreds[idx]) {
    sparseSuccs[p].erase(idx);
  }
  sparseSuccs.erase(idx);
  sparsePreds.erase(idx);
  continuationPoints.erase(idx);
  sparseNodes.erase(idx);
}

void IRICFG::removeUnreachable() {
  // Runs on the sparse (pre-renumber) representation.
  // Cascade: pruning a dead BB's out-edges can strand blocks that were only
  // reachable through it, so recompute until the graph stops shrinking.
  while (true) {
    std::set<BBIDX> reachable;
    std::function<void(BBIDX)> dfs = [&](BBIDX u) {
      if (!reachable.insert(u).second)
        return;
      for (auto s : sparseSuccs[u]) {
        dfs(s);
      }
      auto it = sparseNodes.find(u);
      if (it != sparseNodes.end() && it->second->EXCEPTION != NO_BB) {
        dfs(it->second->EXCEPTION);
      }
    };
    dfs(entry_block);

    std::vector<BBIDX> dead;
    for (auto &e : sparseNodes) {
      if (!reachable.contains(e.first))
        dead.push_back(e.first);
    }
    if (dead.empty())
      break;

    for (auto idx : dead) {
      eraseSparseNode(idx);
    }
    markDirty();
  }
}

void IRICFG::renumberBBs() {
  std::vector<BBIDX> sorted;
  sorted.reserve(sparseNodes.size());
  for (auto &e : sparseNodes)
    sorted.push_back(e.first);
  std::sort(sorted.begin(), sorted.end());

  std::unordered_map<BBIDX, BBIDX> map;
  for (size_t i = 0; i < sorted.size(); i++)
    map[sorted[i]] = (BBIDX)i;

  auto remap = [&](BBIDX oldID) {
    auto it = map.find(oldID);
    if (it == map.end())
      throw std::runtime_error(
          "renumberBBs: edge targets a block outside the CFG");
    return it->second;
  };

  globalEntryID = entry_block;
  entry_block = remap(entry_block);
  exit_block = remap(exit_block);

  // Dense, CFG-local from here on. globalIDs records each block's identity
  // in the program-wide numbering (commit() translates back with it).
  globalIDs = sorted;
  bbs.clear();
  bbs.resize(sorted.size());
  for (auto &e : sparseNodes) {
    e.second->IDX = remap(e.first);
    if (e.second->EXCEPTION != NO_BB)
      e.second->EXCEPTION = remap(e.second->EXCEPTION);
    bbs[e.second->IDX] = std::move(e.second);
  }

  successors.clear();
  successors.resize(sorted.size());
  for (auto &e : sparseSuccs) {
    auto &dest = successors[remap(e.first)];
    for (auto n : e.second)
      dest.insert(remap(n));
  }
  predecessors.clear();
  predecessors.resize(sorted.size());
  for (auto &e : sparsePreds) {
    auto &dest = predecessors[remap(e.first)];
    for (auto n : e.second)
      dest.insert(remap(n));
  }
  sparseNodes.clear();
  sparseSuccs.clear();
  sparsePreds.clear();

  for (auto &bbp : bbs) {
    IRIBB *bb = bbp.get();
    for (IRIStatement *s = bb->head; s != nullptr; s = s->next) {
      if (IRI_NODE(ctx, s->id).tag == IRI_GEN::PushCatchContext)
        PushCatchContextSEXP(s->id, ctx).setIDX(bb->EXCEPTION);
    }
    switch (IRI_NODE(ctx, bb->tail->id).tag) {
    case IRI_GEN::Goto:
      GotoSEXP(bb->tail->id, ctx)
          .setIDX(remap(GotoSEXP(bb->tail->id, ctx).getIDX()));
      bb->setTerminal(bb->tail->id);
      break;
    case IRI_GEN::IfElseJump: {
      IfElseJumpSEXP jump(bb->tail->id, ctx);
      jump.setTRUE(remap(jump.getTRUE()));
      jump.setFALSE(remap(jump.getFALSE()));
      bb->setTerminal(bb->tail->id);
      break;
    }
    default:
      break;
    }
  }

  std::set<BBIDX> remappedCont;
  for (auto c : continuationPoints)
    remappedCont.insert(remap(c));
  continuationPoints = std::move(remappedCont);
}

IRICFG::IRICFG(IRI_STORAGE::IRID i, IRI_STORAGE::IRIContext &p)
    : id(i), ctx(p) {
  BBContainerSupport bbc(id, ctx);
  entry_block = (BBIDX)bbc.getStartBBIDX();

  // Common Return Path
  IRI_STORAGE::StringID commonRet = ctx.storage.strings.intern("<common-ret>");
  retCTX = ctx.iris
               ->declareLBinding(bbc.getScopeIDX(), commonRet,
                                 IRI_GEN::IRI_FLAG::JSVAR)
               .ID;
  IRIBB *exitBB = makeBB(bbc.getScopeIDX());
  exit_block = exitBB->IDX;

  if (bbc.hasASYNC() || bbc.hasGENERATOR()) {
    exitBB->tail = new IRIStatement(
        ReturnAsyncSEXP::create(ctx,
                                EnvReadSEXP::create(ctx, retCTX, true, false)),
        exitBB);
  } else {
    exitBB->tail = new IRIStatement(
        ReturnSEXP::create(ctx, EnvReadSEXP::create(ctx, retCTX, true, false)),
        exitBB);
  }

  // Add all the basic blocks to the graph
  for (auto [bbID, bbOffset] : bbc.bbs()) {
    BBSupport bb(bbID, ctx);
    auto currBBIDX = bb.getIDX();

    sparseNodes[currBBIDX] =
        std::make_unique<IRIBB>(bb.getScopeIDX(), currBBIDX, this, ctx);
  }

  // Populate statements and establish edges
  for (auto [bbID, bbOffset] : bbc.bbs()) {
    BBSupport bbSupp(bbID, ctx);
    auto currBBIDX = (BBIDX)bbSupp.getIDX();
    IRIBB *bb = sparseNodes[currBBIDX].get();
    // Per-BB dispatch target (declared by _20_IFB, carried by clones).
    double dispatch = -1;
    ctx.iris->getExceptionIDX(bbc.getScopeIDX(), currBBIDX, dispatch);
    bb->EXCEPTION = dispatch;
    auto stmts = bbSupp.stmtsVec();
    for (size_t i = 0; i < stmts.size(); i++) {
      IRID stmtID = stmts[i];
      if (i + 1 == stmts.size()) {
        bb->setTerminal(stmtID);
      } else {
        bb->append(new IRIStatement(stmtID, bb));
      }
    }
  }

  // Split BBs at await points
  auto hasAwait = [&](IRID stmtID) {
    return IRI_HELPERS::hasNodeWithPredicate(stmtID, &ctx, [&](IRID id) {
      return IRI_NODE(ctx, id).tag == IRI_GEN::Await;
    });
  };

  std::set<BBIDX> awaitWorklist;
  for (auto &e : sparseNodes) {
    awaitWorklist.insert(e.first);
  }

  // Statements already placed as the first statement of a resume block.
  std::set<IRID> resumeSites;

  while (!awaitWorklist.empty()) {
    BBIDX bbIDX = *awaitWorklist.begin();
    awaitWorklist.erase(awaitWorklist.begin());
    IRIBB *bb = sparseNodes[bbIDX].get();

    if (bb->tail == nullptr) {
      throw std::runtime_error("bb tail cannot be null before split");
    }

    IRIStatement *currStmt = bb->head;
    if (currStmt != nullptr && resumeSites.contains(currStmt->id)) {
      currStmt = currStmt->next;
    }
    while (currStmt != nullptr) {
      if (hasAwait(currStmt->id)) {
        break;
      }
      currStmt = currStmt->next;
    }

    if (currStmt == nullptr) {
      if (hasAwait(bb->tail->id)) {
        throw std::runtime_error(
            "await inside terminal statement; this should not be possible");
      }
      continue;
    }

    IRIBB *postBB = makeBB(bb->SCOPE);
    // Same region, resumed half: inherit the dispatch target.
    postBB->EXCEPTION = bb->EXCEPTION;
    auto postBBIDX = postBB->IDX;
    awaitWorklist.insert(postBBIDX);

    // The await statement itself starts the resume block; the pre block ends
    // carrying only its handle operand across the deferred edge.
    IRIStatement *nextStmts = currStmt;
    if (currStmt->prev != nullptr) {
      currStmt->prev->next = nullptr;
    } else {
      bb->head = nullptr;
    }
    currStmt->prev = nullptr;

    postBB->head = nextStmts;
    if (postBB->head != nullptr) {
      postBB->head->prev = nullptr;
      for (IRIStatement *s = postBB->head; s != nullptr; s = s->next) {
        s->bb = postBB;
      }
    }

    // Terminal + out-edges belong to the resumed half.
    bb->moveTerminalTo(postBB);
    if (postBB->tail == nullptr) {
      throw std::runtime_error("postBB tail cannot be null");
    }

    // Deferred -> true ; this represents a GOTO may act as a early return
    // (useful in analysis that care about suspension points, otherwise its a
    // normal goto)
    bb->setTerminal(GotoSEXP::create(ctx, true, postBBIDX));

    continuationPoints.insert(postBBIDX);
    resumeSites.insert(nextStmts->id);
  }

  removeUnreachable();
  renumberBBs();
  verify();
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
  for (auto &bb : bbs) {
    nodesTODO.insert(bb->IDX);
  }

  std::function<void(BBIDX)> dumpRecurse = [&](BBIDX currentBBIDX) {
    if (!nodesTODO.contains(currentBBIDX))
      return;
    nodesTODO.erase(currentBBIDX);

    std::stringstream ss;
    IRIBB *bb = get_bb(currentBBIDX);
    bb->dumpFlat(ss);

    bool isCont = continuationPoints.contains(currentBBIDX);
    std::string xlabel = "BB" + std::to_string((int)currentBBIDX) +
                         (isCont ? " (Continuation)" : "");
    std::string extraStyle =
        isCont ? ", fillcolor=\"#e1f5fe\", color=\"#0288d1\"" : "";

    outFile << "  " << (int)currentBBIDX << " [label=\"" << ss.str()
            << "\", xlabel=\"" << xlabel << "\"" << extraStyle << "];\n";

    for (auto succ : successorsOf(currentBBIDX)) {
      outFile << "  " << (int)currentBBIDX << " -> " << (int)succ << ";\n";
      dumpRecurse(succ);
    }

    if (bb->EXCEPTION != NO_BB) {
      outFile << "  " << (int)currentBBIDX << " -> " << (int)bb->EXCEPTION
              << " [style=\"dotted\", color=\"red\"];\n";
      dumpRecurse(bb->EXCEPTION);
    }
  };
  dumpRecurse(entry_block);

  for (auto contIDX : continuationPoints) {
    dumpRecurse(contIDX);
  }

  for (auto &wasteBB : nodesTODO) {
    std::cerr << "  DeadBB: " << wasteBB << std::endl;
  }

  outFile << "}\n";
}

void IRICFG::commit() {
  verify();

  std::vector<IRID> newBBS;
  BBContainerSupport bbc(id, ctx);
  IRID newBBList = ListSEXP::create(ctx, ctx.storage.strings.intern("BB"));
  bbc.setArg_BB(newBBList);

  std::set<BBIDX> nodesTODO;
  for (auto &bb : bbs) {
    nodesTODO.insert(bb->IDX);
  }

  // The CFG speaks dense local handles; the core IR keeps the original
  // program-wide numbering, so blocks and jump targets are translated back
  // on the way out (commit is the local->global boundary).
  std::function<void(BBIDX)> dumpRecurse = [&](BBIDX currentBBIDX) {
    if (!nodesTODO.contains(currentBBIDX))
      return;
    nodesTODO.erase(currentBBIDX);

    IRIBB *bb = get_bb(currentBBIDX);
    IRID currBBID = BBSEXP::create(ctx, false, false, false, false, false,
                                   bb->SCOPE, globalBBID(bb->IDX));
    newBBS.push_back(currBBID);

    std::vector<IRID> stmtList;

    IRIStatement *curr = bb->head;
    while (curr != nullptr) {
      if (IRI_NODE(ctx, curr->id).tag == IRI_GEN::PushCatchContext) {
        PushCatchContextSEXP(curr->id, ctx).setIDX(globalBBID(bb->EXCEPTION));
      }
      stmtList.push_back(curr->id);
      curr = curr->next;
    }
    if (bb->tail != nullptr) {
      switch (IRI_NODE(ctx, bb->tail->id).tag) {
      case IRI_GEN::Goto:
        GotoSEXP(bb->tail->id, ctx)
            .setIDX(globalBBID(GotoSEXP(bb->tail->id, ctx).getIDX()));
        break;
      case IRI_GEN::IfElseJump: {
        IfElseJumpSEXP jump(bb->tail->id, ctx);
        jump.setTRUE(globalBBID(jump.getTRUE()));
        jump.setFALSE(globalBBID(jump.getFALSE()));
        break;
      }
      default:
        break;
      }
      stmtList.push_back(bb->tail->id);
    }

    ctx.storage.nodes.set_args(currBBID, stmtList);

    for (auto succ : successorsOf(currentBBIDX)) {
      dumpRecurse(succ);
    }

    if (bb->EXCEPTION != NO_BB) {
      dumpRecurse(bb->EXCEPTION);
    }
  };
  dumpRecurse(entry_block);
  ctx.storage.nodes.set_args(newBBList, newBBS);
}

const PTAClosureInfo &IRICFG::ptaInfo() {
  if (ptaInfoCache.has_value())
    return *ptaInfoCache;

  BBContainerSupport bbc(id, ctx);
  PTAClosureInfo info;

  // JSCTX only ever appears as the RVal of an LWrite
  for (auto &bbp : bbs) {
    for (IRIStatement *s = bbp->head; s != nullptr; s = s->next) {
      if (IRI_NODE(ctx, s->id).tag != IRI_GEN::LWrite)
        continue;
      IRID rval = IRI_GEN::LWriteSEXP(s->id, ctx).getArg_RVal();
      if (IRI_NODE(ctx, rval).tag == IRI_GEN::JSCTX)
        info.usedJSCTXSlots.insert(IRI_GEN::JSCTXSEXP(rval, ctx).getOPID());
    }
  }

  // 0 - ArgumentsObject
  // 1 - MappedArgumentsObject
  bool argsEscape = info.usedJSCTXSlots.contains(1);
  for (const auto b : ctx.iris->getEnvBindingsInClosure(bbc.getScopeIDX())) {
    IRI_GEN::EnvBindingSEXP eb(b, ctx);
    if (eb.hasJSARG())
      info.formals.push_back({eb.getREFIDX(), b});

    // Bindings can escape if they are either captured by an inner closure or
    // the frame actually uses a mapped ArgumentsObject, in which case it holds
    // references to argument bindings internally
    bool captured =
        IRI_BINDING(ctx.iris, b).isCaptured() || (argsEscape && eb.hasJSARG());
    (captured ? info.tstack : info.stack).push_back(b);
  }
  std::sort(info.formals.begin(), info.formals.end());

  auto remoteBindings =
      ctx.iris->getRemoteEnvBindingsInClosure(bbc.getScopeIDX());
  for (const auto rb : remoteBindings)
    info.remoteRefs.push_back(IRI_HELPERS::resolveRemoteBinding(ctx, rb));

  if (ctx.iris->isTopLevelScope(bbc.getScopeIDX())) {
    // A module's top level owns its RemoteEnvBindings, a script its
    // ScriptBindings; both are plain stack cells here.
    for (const auto b : remoteBindings)
      info.stack.push_back(b);
    for (const auto b : ctx.iris->getScriptBindings())
      info.stack.push_back(b);
    info.globals = ctx.iris->getGlobalBindings();
  }

  ptaInfoCache = std::move(info);
  return *ptaInfoCache;
}

std::string IRIBB::getDebugID() {
  return closure->getDebugID() + "/bb" + std::to_string((int)IDX);
}

std::string IRIBB::getDebugName() {
  if (IDX == closure->entry_block)
    return "Entry";
  else if (IDX == closure->exit_block)
    return "Exit";
  else
    return getDebugID();
}

std::string IRIStatement::getDebugID() {
  return bb->getDebugID() + "/i" + bb->getInstID(this);
}

std::string IRICFG::getDebugID() {
  BBContainerSEXP bbc(id, ctx);
  return "c" + std::to_string((int)bbc.getStartBBIDX());
}

std::string IRICFG::getDebugName() {
  BBContainerSEXP bbc(id, ctx);
  return std::string(ctx.storage.strings.get(bbc.getNAME()));
}

std::vector<BBIDX> IRICFG::getReversePostOrder(bool includeExceptions) const {
  return getReversePostOrder(includeExceptions, entry_block);
}

std::vector<BBIDX> IRICFG::getReversePostOrder(bool includeExceptions,
                                               BBIDX startBBIDX) const {
  std::vector<BBIDX> postOrder;
  std::set<BBIDX> visited;

  std::function<void(BBIDX)> dfs = [&](BBIDX u) {
    visited.insert(u);
    for (BBIDX v : successorsOf(u)) {
      if (!visited.contains(v)) {
        dfs(v);
      }
    }
    if (includeExceptions && u >= 0 && (size_t)u < bbs.size() && bbs[u]) {
      BBIDX exc = bbs[u]->EXCEPTION;
      if (exc != NO_BB && !visited.contains(exc)) {
        dfs(exc);
      }
    }
    postOrder.push_back(u);
  };

  dfs(startBBIDX);

  std::reverse(postOrder.begin(), postOrder.end());
  return postOrder;
}

} // namespace IRI_STRUCTURAL
