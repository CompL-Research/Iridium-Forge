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

IRIBB::IRIBB(double ss, double i, IRICFG *c, IRI_STORAGE::IRIContext &p)
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

void IRIBB::clearEdges() {
  for (auto &e : closure->successors[IDX]) {
    closure->predecessors[e].erase(IDX);
  }
  closure->successors[IDX].clear();
}

void IRIBB::addEdge(BBIDX succ) {
  closure->successors[IDX].insert(succ);
  closure->predecessors[succ].insert(IDX);
}

void IRIBB::moveTerminalTo(IRIBB *dst) {
  if (tail == nullptr) {
    throw std::runtime_error("moveTerminalTo: source has no terminal");
  }
  if (dst->tail != nullptr) {
    throw std::runtime_error("moveTerminalTo: dst already has a terminal");
  }

  bool wasRet = IRI_NODE(ctx, tail->id).tag == IRI_GEN::Ret;

  dst->tail = tail;
  dst->tail->bb = dst;
  dst->tail->prev = nullptr;
  dst->tail->next = nullptr;
  tail = nullptr;

  auto succs = closure->successors[IDX];
  clearEdges();
  for (auto s : succs) {
    dst->addEdge(s);
  }

  dst->FINTARGET = FINTARGET;
  FINTARGET = -1;

  // A finalizer body's Ret may be pushed into a fresh BB by a later split;
  // whoever asks for that body's exit must now be told about the new owner.
  if (wasRet) {
    closure->retagFinalizerRet(IDX, dst->IDX);
  }
}

IRIBB *IRICFG::makeBB(ScopeIDX scope) {
  auto idx = ++ctx.lastBBIDX;
  nodeMap[idx] = std::make_unique<IRIBB>(scope, idx, this, ctx);
  IRIBB *bb = nodeMap[idx].get();
  // Was missing on finalizer-split post-blocks: they inherit the scope, so
  // they must inherit the handler too.
  bb->EXCEPTION = ctx.iris->getExceptionTargetForScope(scope);
  return bb;
}

BBIDX IRICFG::finalizerRetOf(BBIDX entryIDX) {
  auto it = finalizerRetBB.find(entryIDX);
  if (it != finalizerRetBB.end()) {
    return it->second;
  }
  BBIDX ret = ctx.iris->getFinalizerRetBBIDX(entryIDX);
  finalizerRetBB[entryIDX] = ret;
  return ret;
}

void IRICFG::retagFinalizerRet(BBIDX from, BBIDX to) {
  for (auto &e : finalizerRetBB) {
    if (e.second == from) {
      e.second = to;
    }
  }
}

void IRIBB::setTerminal(IRID stmtID) {
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
  case IRI_GEN::InvokeFinalizer: {
    tail = new IRIStatement(stmtID, this);
    InvokeFinalizerSEXP inv(stmtID, ctx);
    addEdge(inv.getIDX());
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
  for (auto &e : nodeMap) {
    IRIBB *bb = e.second.get();

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
      curr = curr->next;
    }
    if (bb->tail->bb != bb) {
      throw std::runtime_error("terminal bb back-pointer is stale");
    }

    // This is the invariant that broke: a Ret with no dispatch set means its
    // invoke sites became unreachable and commit() will silently drop them.
    if (IRI_NODE(ctx, bb->tail->id).tag == IRI_GEN::Ret &&
        successors[bb->IDX].empty()) {
      throw std::runtime_error("Ret terminal with empty dispatch set");
    }

    for (auto s : successors[bb->IDX]) {
      if (!nodeMap.contains(s)) {
        throw std::runtime_error("successor missing from nodeMap");
      }
      if (!predecessors[s].contains(bb->IDX)) {
        throw std::runtime_error("successor/predecessor asymmetry");
      }
    }
    if (bb->FINTARGET > -1 && !nodeMap.contains(bb->FINTARGET)) {
      throw std::runtime_error("FINTARGET missing from nodeMap");
    }
    if (bb->EXCEPTION > -1 && !nodeMap.contains(bb->EXCEPTION)) {
      throw std::runtime_error("EXCEPTION missing from nodeMap");
    }
  }
}

IRICFG::IRICFG(IRI_STORAGE::IRID i, IRI_STORAGE::IRIContext &p)
    : id(i), ctx(p) {
  BBContainerSupport bbc(id, ctx);
  // Entry BBIDX
  bool wasEntrySet = false;
  entry_block = bbc.getStartBBIDX();

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

    nodeMap[currBBIDX] =
        std::make_unique<IRIBB>(bb.getScopeIDX(), currBBIDX, this, ctx);
    if (currBBIDX == entry_block) {
      assert(!wasEntrySet);
      wasEntrySet = true;
    }
  }

  assert(wasEntrySet);

  // Populate statements and establish edges
  for (auto [bbID, bbOffset] : bbc.bbs()) {
    BBSupport bbSupp(bbID, ctx);
    auto currBBIDX = bbSupp.getIDX();
    IRIBB *bb = nodeMap[currBBIDX].get();
    bb->EXCEPTION = ctx.iris->getExceptionTargetForScope(bb->SCOPE);
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

  // Split BBs at Finalizer Calls
  //
  // Worklists are keyed on BBIDX, not IRIBB*: pointer ordering makes split
  // order (and therefore every synthetic BBIDX) vary run to run.
  std::set<BBIDX> worklist;
  for (auto &e : nodeMap) {
    worklist.insert(e.first);
  }
  while (!worklist.empty()) {
    BBIDX bbIDX = *worklist.begin();
    worklist.erase(worklist.begin());
    IRIBB *bb = nodeMap[bbIDX].get();

    IRIStatement *curr = bb->head;
    while (curr != nullptr) {
      if (IRI_NODE(ctx, curr->id).tag == IRI_GEN::InvokeFinalizer) {
        break;
      }
      curr = curr->next;
    }

    if (curr != nullptr) {
      InvokeFinalizerSEXP ivTarget(curr->id, ctx);
      auto finalizerEntryIDX = ivTarget.getIDX();
      auto finalizerExitIDX = finalizerRetOf(finalizerEntryIDX);
      if (!nodeMap.contains(finalizerEntryIDX)) {
        throw std::runtime_error("finalizerEntryIDX missing");
      }
      if (!nodeMap.contains(finalizerExitIDX)) {
        throw std::runtime_error("finalizerExitIDX missing");
      }

      IRIBB *postBB = makeBB(bb->SCOPE);
      auto postBBIDX = postBB->IDX;
      worklist.insert(postBBIDX);

      // Everything after the invoke moves to postBB...
      postBB->head = curr->next;
      if (postBB->head != nullptr) {
        postBB->head->prev = nullptr;
        for (IRIStatement *s = postBB->head; s != nullptr; s = s->next) {
          s->bb = postBB;
        }
      }
      // ...including the terminal, its out-edges and its FINTARGET.
      bb->moveTerminalTo(postBB);

      // Make the prev stmt the last
      if (curr->prev) {
        curr->prev->next = nullptr;
      } else {
        bb->head = nullptr;
      }

      // Set the invoke finalizer as the terminal. The bb -> finalizerEntry
      // edge is registered by setTerminal now, no manual fixup.
      bb->setTerminal(curr->id);
      delete curr;

      bool finalizerDynamicRet =
          IRI_NODE(ctx, nodeMap[finalizerExitIDX]->tail->id).tag == Ret;
      if (finalizerDynamicRet) {
        bb->FINTARGET = postBBIDX;
        // Edge from the finalizer body's Ret back to this invoke's post block.
        nodeMap[finalizerExitIDX]->addEdge(postBBIDX);
      } else {
        bb->FINTARGET = -2;
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
  for (auto &e : nodeMap) {
    awaitWorklist.insert(e.first);
  }

  while (!awaitWorklist.empty()) {
    BBIDX bbIDX = *awaitWorklist.begin();
    awaitWorklist.erase(awaitWorklist.begin());
    IRIBB *bb = nodeMap[bbIDX].get();

    if (bb->tail == nullptr) {
      throw std::runtime_error("bb tail cannot be null before split");
    }

    IRIStatement *currStmt = bb->head;
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
    auto postBBIDX = postBB->IDX;
    awaitWorklist.insert(postBBIDX);

    IRIStatement *nextStmts = currStmt->next;
    currStmt->next = nullptr;

    postBB->head = nextStmts;
    if (postBB->head != nullptr) {
      postBB->head->prev = nullptr;
      for (IRIStatement *s = postBB->head; s != nullptr; s = s->next) {
        s->bb = postBB;
      }
    }

    // Terminal + out-edges + FINTARGET all belong to the resumed half.
    bb->moveTerminalTo(postBB);
    if (postBB->tail == nullptr) {
      throw std::runtime_error("postBB tail cannot be null");
    }

    // Deferred -> true ; this represents a GOTO may act as a early return
    // (useful in analysis that care about suspension points, otherwise its a
    // normal goto)
    bb->setTerminal(GotoSEXP::create(ctx, true, postBBIDX));

    continuationPoints.insert(postBBIDX);
  }

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

    bool isCont = continuationPoints.contains(currentBBIDX);
    std::string xlabel = "BB" + std::to_string((int)currentBBIDX) +
                         (isCont ? " (Continuation)" : "");
    std::string extraStyle =
        isCont ? ", fillcolor=\"#e1f5fe\", color=\"#0288d1\"" : "";

    outFile << "  " << (int)currentBBIDX << " [label=\"" << ss.str()
            << "\", xlabel=\"" << xlabel << "\"" << extraStyle << "];\n";

    for (auto succ : successors[currentBBIDX]) {
      outFile << "  " << (int)currentBBIDX << " -> " << (int)succ << ";\n";
      dumpRecurse(succ);
    }

    if (bb->EXCEPTION > -1) {
      outFile << "  " << (int)currentBBIDX << " -> " << (int)bb->EXCEPTION
              << " [style=\"dotted\", color=\"red\"];\n";
      dumpRecurse(bb->EXCEPTION);
    }

    if (bb->FINTARGET > -1) {
      outFile << "  " << (int)currentBBIDX << " -> " << (int)bb->FINTARGET
              << " [style=\"dotted\"];\n";
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
  std::vector<IRID> newBBS;
  BBContainerSupport bbc(id, ctx);
  IRID newBBList = ListSEXP::create(ctx, ctx.storage.strings.intern("BB"));
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
    IRID currBBID = BBSEXP::create(ctx, false, false, false, false, false,
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
      stmtList.push_back(ThrowSEXP::create(
          ctx,
          StringSEXP::create(ctx, ctx.storage.strings.intern("Unreachable"))));
    } else if (bb->FINTARGET > -1) {
      stmtList.push_back(GotoSEXP::create(ctx, false, bb->FINTARGET));
    }

    ctx.storage.nodes.set_args(currBBID, stmtList);

    for (auto succ : successors[currentBBIDX]) {
      dumpRecurse(succ);
    }

    if (bb->EXCEPTION > -1) {
      dumpRecurse(bb->EXCEPTION);
    }
  };
  dumpRecurse(entry_block);

  ctx.storage.nodes.set_args(newBBList, newBBS);
}

std::vector<IRID> IRICFG::ptaGenStack() {
  BBContainerSupport bbc(id, ctx);

  // Local stack bindings
  auto bindings = ctx.iris->getEnvBindingsInClosure(bbc.getScopeIDX());
  std::vector<IRID> res;
  for (const auto b : bindings) {
    if (!IRI_BINDING(ctx.iris, b).isCaptured()) {
      res.push_back(b);
    }
  }

  // Module / Script level bindings
  if (ctx.iris->isTopLevelScope(bbc.getScopeIDX())) {
    // Module has top level RemoteEnvBindings
    auto rBindings = ctx.iris->getRemoteEnvBindingsInClosure(bbc.getScopeIDX());
    for (const auto b : rBindings) {
      res.push_back(b);
    }

    // Script has ScriptBindings
    auto sBindings = ctx.iris->getScriptBindings();
    for (const auto b : sBindings) {
      res.push_back(b);
    }
  }
  return res;
}

std::vector<IRID> IRICFG::ptaGenTStack() {
  BBContainerSupport bbc(id, ctx);
  auto bindings = ctx.iris->getEnvBindingsInClosure(bbc.getScopeIDX());
  std::vector<IRID> res;
  for (const auto b : bindings) {
    if (IRI_BINDING(ctx.iris, b).isCaptured()) {
      res.push_back(b);
    }
  }
  return res;
}

std::vector<IRID> IRICFG::ptaAssertTransient() {
  BBContainerSupport bbc(id, ctx);
  auto bindings = ctx.iris->getRemoteEnvBindingsInClosure(bbc.getScopeIDX());
  std::vector<IRID> res;
  for (const auto rb : bindings) {
    IRID b = IRI_HELPERS::resolveRemoteBinding(ctx, rb);
    res.push_back(b);
  }
  return res;
}

std::vector<IRID> IRICFG::ptaAssertGlobals() {
  BBContainerSupport bbc(id, ctx);
  if (!ctx.iris->isTopLevelScope(bbc.getScopeIDX()))
    return {};

  return ctx.iris->getGlobalBindings();
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
                                               double startBBIDX) const {
  std::vector<BBIDX> postOrder;
  std::set<BBIDX> visited;

  std::function<void(BBIDX)> dfs = [&](BBIDX u) {
    visited.insert(u);
    auto it = successors.find(u);
    if (it != successors.end()) {
      for (BBIDX v : it->second) {
        if (!visited.contains(v)) {
          dfs(v);
        }
      }
    }
    if (includeExceptions) {
      auto nodeIt = nodeMap.find(u);
      if (nodeIt != nodeMap.end() && nodeIt->second->EXCEPTION > -1) {
        BBIDX exc = nodeIt->second->EXCEPTION;
        if (!visited.contains(exc)) {
          dfs(exc);
        }
      }
    }
    postOrder.push_back(u);
  };

  dfs(startBBIDX);

  std::reverse(postOrder.begin(), postOrder.end());
  return postOrder;
}

} // namespace IRI_STRUCTURAL
