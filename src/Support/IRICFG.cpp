#include "Support/IRICFG.hpp"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Storage/IRIContext.h"
#include "Support/BBContainerSupport.hpp"
#include "Support/BBSupport.hpp"
#include "Support/IRIS.hpp"
#include <algorithm>
#include <functional>
#include <set>
#include <stdexcept>
#include <unordered_map>

namespace IRI_STRUCTURAL {
using namespace IRI_STORAGE;
using namespace IRI_GEN;

namespace {

//
// Helpers
//
struct Shape {
  BBIDX entry;
  BBIDX exit;
  IRIContext &ctx;
};

bool isTerminator(IRI_TAG tag) {
  return tag == Goto || tag == IfElseJump || tag == Throw || tag == Return ||
         tag == ReturnAsync;
}

bool isControlFlow(IRI_TAG tag) { return isTerminator(tag) || tag == IfJump; }

void erasePred(std::vector<IRIBB> &blocks, BBIDX target, BBIDX source,
               EdgeKind kind) {
  std::erase_if(blocks[target].preds,
                [&](Edge e) { return e.to == source && e.kind == kind; });
}

void deriveEdges(std::vector<IRIBB> &blocks, BBIDX h, IRID terminal,
                 const Shape &shape) {
  IRIBB &bb = blocks[h];

  for (Edge e : bb.succs)
    if (e.kind != EdgeKind::Exception)
      erasePred(blocks, e.to, h, e.kind);
  std::erase_if(bb.succs, [](Edge e) { return e.kind != EdgeKind::Exception; });

  auto link = [&](BBIDX to, EdgeKind kind) {
    if (to == shape.entry)
      throw std::runtime_error("deriveEdges: terminal targets ENTRY");
    if (to >= blocks.size())
      throw std::runtime_error(
          "deriveEdges: target is not a block of this CFG");
    bb.succs.push_back({to, kind});
    blocks[to].preds.push_back({h, kind});
  };

  IRIContext &ctx = shape.ctx;
  switch (IRI_NODE(ctx, terminal).tag) {
  case Goto: {
    GotoSEXP gotoSEXP(terminal, ctx);
    link((BBIDX)gotoSEXP.getIDX(),
         gotoSEXP.hasDeferred() ? EdgeKind::Deferred : EdgeKind::Normal);
    break;
  }
  case IfElseJump: {
    IfElseJumpSEXP jump(terminal, ctx);
    link((BBIDX)jump.getTRUE(), EdgeKind::Normal);
    link((BBIDX)jump.getFALSE(), EdgeKind::Normal);
    break;
  }
  case Throw:
    // A throw that escapes the closure is how the closure ends.
    link(shape.exit, EdgeKind::Normal);
    break;
  case Return:
  case ReturnAsync:
    if (h != shape.exit)
      throw std::runtime_error("deriveEdges: Return outside EXIT");
    break;
  default:
    throw std::runtime_error("deriveEdges: terminal is not a terminator");
  }
}

void setExceptionEdge(std::vector<IRIBB> &blocks, BBIDX h, BBIDX target,
                      const Shape &shape) {
  if (target == h)
    throw std::runtime_error("setExceptionEdge: exception edge to self");
  if (target >= blocks.size())
    throw std::runtime_error("setExceptionEdge: target is not a block");
  if (target == shape.entry)
    throw std::runtime_error("setExceptionEdge: exception edge targets ENTRY");
  if (h == shape.exit)
    throw std::runtime_error("setExceptionEdge: EXIT has an exception edge");

  IRIBB &bb = blocks[h];
  for (Edge e : bb.succs)
    if (e.kind == EdgeKind::Exception)
      erasePred(blocks, e.to, h, EdgeKind::Exception);
  std::erase_if(bb.succs, [](Edge e) { return e.kind == EdgeKind::Exception; });

  bb.succs.push_back({target, EdgeKind::Exception});
  blocks[target].preds.push_back({h, EdgeKind::Exception});
}

BBIDX exceptionEdge(const std::vector<IRIBB> &blocks, BBIDX h) {
  for (Edge e : blocks[h].succs)
    if (e.kind == EdgeKind::Exception)
      return e.to;
  return NO_BB;
}

void moveOutEdges(std::vector<IRIBB> &blocks, BBIDX from, BBIDX to) {
  for (Edge e : blocks[from].succs)
    for (Edge &p : blocks[e.to].preds)
      if (p.to == from)
        p.to = to;
  blocks[to].succs = std::move(blocks[from].succs);
  blocks[from].succs.clear();
}

//
// 1.1 Validate the incoming CFG
//
void validateInput(BBContainerSupport &bbc, IRIContext &ctx) {
  std::set<BBIDX> declared;
  for (auto [bbID, off] : bbc.bbs())
    declared.insert((BBIDX)BBSupport(bbID, ctx).getIDX());

  BBIDX start = (BBIDX)bbc.getStartBBIDX();
  auto named = [&](BBIDX target, const char *what) {
    if (!declared.contains(target))
      throw std::runtime_error(std::string("IRICFG input: ") + what +
                               " names a block outside this closure");
    if (target == start)
      throw std::runtime_error(std::string("IRICFG input: ") + what +
                               " targets ENTRY");
  };

  for (auto [bbID, off] : bbc.bbs()) {
    BBSupport bb(bbID, ctx);
    if (bb.hasEXCIDX())
      named((BBIDX)bb.getEXCIDX(), "EXCIDX");

    auto stmts = bb.stmtsVec();
    if (stmts.empty())
      throw std::runtime_error("IRICFG input: BB with no statements");

    for (size_t i = 0; i + 1 < stmts.size(); i++) {
      IRI_TAG tag = IRI_NODE(ctx, stmts[i]).tag;
      if (isControlFlow(tag))
        throw std::runtime_error("IRICFG input: control flow mid-block");

      // Probably redundant...? ~Meetesh
      if (tag == Ret || tag == InvokeFinalizer)
        throw std::runtime_error("IRICFG input: a tag the inline-finalizer "
                                 "pass should have removed");
      if (tag == PushCatchContext)
        named((BBIDX)PushCatchContextSEXP(stmts[i], ctx).getIDX(),
              "PushCatchContext");
    }

    IRID terminal = stmts.back();
    IRI_TAG tag = IRI_NODE(ctx, terminal).tag;
    if (!isTerminator(tag))
      throw std::runtime_error("IRICFG input: BB does not end in a terminator");
    if (tag == Goto)
      named((BBIDX)GotoSEXP(terminal, ctx).getIDX(), "Goto");
    else if (tag == IfElseJump) {
      IfElseJumpSEXP jump(terminal, ctx);
      named((BBIDX)jump.getTRUE(), "IfElseJump.TRUE");
      named((BBIDX)jump.getFALSE(), "IfElseJump.FALSE");
    }
  }
}

//
// 1.2 Remove unreachable blocks
//
std::vector<BBIDX> liveBlocks(BBContainerSupport &bbc, IRIContext &ctx) {
  std::unordered_map<BBIDX, IRID> byIDX;
  for (auto [bbID, off] : bbc.bbs())
    byIDX[(BBIDX)BBSupport(bbID, ctx).getIDX()] = bbID;

  std::set<BBIDX> seen;
  std::function<void(BBIDX)> walk = [&](BBIDX u) {
    if (!seen.insert(u).second)
      return;

    BBSupport bb(byIDX.at(u), ctx);
    IRID terminal = bb.stmtsVec().back();
    switch (IRI_NODE(ctx, terminal).tag) {
    case Goto:
      walk((BBIDX)GotoSEXP(terminal, ctx).getIDX());
      break;
    case IfElseJump: {
      IfElseJumpSEXP jump(terminal, ctx);
      walk((BBIDX)jump.getTRUE());
      walk((BBIDX)jump.getFALSE());
      break;
    }
    case Throw:
    case Return:
    case ReturnAsync:
      break;
    default:
      throw std::runtime_error("liveBlocks: BB does not end in a terminator");
    }
    if (bb.hasEXCIDX())
      walk((BBIDX)bb.getEXCIDX());
  };

  walk((BBIDX)bbc.getStartBBIDX());
  return std::vector<BBIDX>(seen.begin(), seen.end());
}

//
// 1.3 Canonical Exit Point for the CFG
//
IRID makeExitBlock(std::vector<IRIBB> &blocks, const Shape &shape, SCPIDX scope,
                   bool resumable) {
  IRID retCTX = shape.ctx.iris
                    ->declareLBinding(
                        scope, shape.ctx.storage.strings.intern("<common-ret>"),
                        IRI_GEN::IRI_FLAG::JSVAR)
                    .ID;

  IRIBB &exitBB = blocks[shape.exit];
  exitBB.SCOPE = scope;
  IRID read = EnvReadSEXP::create(shape.ctx, retCTX, true, false);
  exitBB.terminal = resumable ? ReturnAsyncSEXP::create(shape.ctx, read)
                              : ReturnSEXP::create(shape.ctx, read);
  deriveEdges(blocks, shape.exit, exitBB.terminal, shape);
  return retCTX;
}

//
// 1.4 Construct the new CFG, add edges, etc... (skip returns)
//
void extractBlocks(BBContainerSupport &bbc,
                   const std::unordered_map<BBIDX, BBIDX> &handle,
                   std::vector<IRIBB> &blocks, const Shape &shape) {
  IRIContext &ctx = shape.ctx;
  auto local = [&](BBIDX programID) {
    auto it = handle.find(programID);
    if (it == handle.end())
      throw std::runtime_error(
          "extractBlocks: a pruned block is still referenced");
    return it->second;
  };

  for (auto [programID, h] : handle) {
    BBSupport bb(bbc.getBBByIDX(programID), ctx);
    IRIBB &dst = blocks[h];
    dst.SCOPE = bb.getScopeIDX();

    if (bb.hasEXCIDX())
      setExceptionEdge(blocks, h, local((BBIDX)bb.getEXCIDX()), shape);

    auto stmts = bb.stmtsVec();
    dst.stmts.assign(stmts.begin(), stmts.end() - 1);
    dst.terminal = stmts.back();

    for (IRID st : dst.stmts)
      if (IRI_NODE(ctx, st).tag == PushCatchContext) {
        PushCatchContextSEXP pcc(st, ctx);
        pcc.setIDX(local((BBIDX)pcc.getIDX()));
      }

    switch (IRI_NODE(ctx, dst.terminal).tag) {
    case Goto: {
      GotoSEXP gotoSEXP(dst.terminal, ctx);
      gotoSEXP.setIDX(local((BBIDX)gotoSEXP.getIDX()));
      break;
    }
    case IfElseJump: {
      IfElseJumpSEXP jump(dst.terminal, ctx);
      jump.setTRUE(local((BBIDX)jump.getTRUE()));
      jump.setFALSE(local((BBIDX)jump.getFALSE()));
      break;
    }
    case Throw:
      break;
    case Return:
    case ReturnAsync:
      // Moved to a different step...
      continue;
    default:
      throw std::runtime_error("extractBlocks: not a terminator");
    }

    deriveEdges(blocks, h, dst.terminal, shape);
  }
}

//
// 1.5 Link the canonical Exit point to all early exits
//
void normalizeReturns(std::vector<IRIBB> &blocks, const Shape &shape,
                      IRID retCTX) {
  IRIContext &ctx = shape.ctx;
  for (size_t i = 0; i < blocks.size(); i++) {
    BBIDX h = (BBIDX)i;
    if (h == shape.exit)
      continue;
    IRID &terminal = blocks[h].terminal;
    IRID rval = NO_IRID;
    if (IRI_NODE(ctx, terminal).tag == Return)
      rval = ReturnSEXP(terminal, ctx).getArg_Obj();
    else if (IRI_NODE(ctx, terminal).tag == ReturnAsync)
      rval = ReturnAsyncSEXP(terminal, ctx).getArg_RetVal();
    else
      continue;

    blocks[h].stmts.push_back(LWriteSEXP::create(ctx, retCTX, rval, false));
    terminal = GotoSEXP::create(ctx, false, shape.exit);
    deriveEdges(blocks, h, terminal, shape);
  }
}

//
// 1.6 Split at awaits (await moves inside the split BB now; will be useful for
// PTA)
//
void splitAwaits(std::vector<IRIBB> &blocks, const Shape &shape) {
  IRIContext &ctx = shape.ctx;
  auto hasAwait = [&](IRID stmtID) {
    return IRI_HELPERS::hasNodeWithPredicate(
        stmtID, &ctx, [&](IRID id) { return IRI_NODE(ctx, id).tag == Await; });
  };

  std::set<IRID> resumeSites;
  std::vector<BBIDX> worklist;
  for (size_t i = 0; i < blocks.size(); i++)
    worklist.push_back((BBIDX)i);

  for (size_t i = 0; i < worklist.size(); i++) {
    BBIDX h = worklist[i];
    std::vector<IRID> &stmts = blocks[h].stmts;

    size_t split = 0;
    bool found = false;
    // Annoying bug, went into infinite loop because we were splitting
    // at the prebiously splitted await inside the splitted BB
    size_t from = (!stmts.empty() && resumeSites.contains(stmts[0])) ? 1 : 0;
    for (size_t k = from; k < stmts.size(); k++)
      if (hasAwait(stmts[k])) {
        split = k;
        found = true;
        break;
      }
    if (!found) {
      if (hasAwait(blocks[h].terminal))
        throw std::runtime_error(
            "splitAwaits: await inside a terminal statement");
      continue;
    }

    BBIDX post = (BBIDX)blocks.size();
    blocks.push_back(IRIBB{});
    // Most annoying feature awaird in C++ award goes to... moved vectors after
    // push
    IRIBB &pre = blocks[h];
    IRIBB &postBB = blocks[post];

    postBB.SCOPE = pre.SCOPE;
    postBB.stmts.assign(pre.stmts.begin() + split, pre.stmts.end());
    pre.stmts.erase(pre.stmts.begin() + split, pre.stmts.end());

    BBIDX handler = exceptionEdge(blocks, h);

    // Terminal and out-edges move together: control flow is the terminal's.
    // This carries the Exception edge to the resumed half as well.
    postBB.terminal = pre.terminal;
    pre.terminal = NO_IRID;
    moveOutEdges(blocks, h, post);

    // Both halves can throw, so both need the dispatch target: moveOutEdges
    // took it from the pre half, so give it back.
    if (handler != NO_BB)
      setExceptionEdge(blocks, h, handler, shape);

    // DEFERRED is what makes this edge a Deferred one. The old code set the
    // flag and then filed the edge as a normal successor, which every
    // suspension-aware analysis had to work around.
    pre.terminal = GotoSEXP::create(ctx, true, post);
    deriveEdges(blocks, h, pre.terminal, shape);
    deriveEdges(blocks, post, postBB.terminal, shape);

    resumeSites.insert(postBB.stmts.front());
    worklist.push_back(post);
  }
}

} // namespace

IRICFG::IRICFG(IRID i, IRIContext &p) : ctx(p), container(i) {
  BBContainerSupport bbc(container, ctx);
  validateInput(bbc, ctx);

  std::vector<BBIDX> live = liveBlocks(bbc, ctx);

  std::unordered_map<BBIDX, BBIDX> handle;
  for (size_t h = 0; h < live.size(); h++)
    handle[live[h]] = (BBIDX)(h + 1);
  entry = handle.at((BBIDX)bbc.getStartBBIDX());
  exit = 0;
  Shape shape{entry, exit, ctx};

  blocks.resize(live.size() + 1);
  IRID retCTX = makeExitBlock(blocks, shape, bbc.getScopeIDX(),
                              bbc.hasASYNC() || bbc.hasGENERATOR());

  extractBlocks(bbc, handle, blocks, shape);
  normalizeReturns(blocks, shape, retCTX);
  splitAwaits(blocks, shape);

  for (size_t h = 0; h < blocks.size(); h++)
    if (blocks[h].terminal == NO_IRID)
      throw std::runtime_error("IRICFG: block has no terminal");
}

// ---------------------------------------------------------------------------
// Read access
// ---------------------------------------------------------------------------

const IRIBB &IRICFG::at(BBIDX h) const {
  if (h == NO_BB || (size_t)h >= blocks.size())
    throw std::runtime_error("IRICFG: handle " + std::to_string(h) +
                             " is not a block of " + debugID() + " (" +
                             std::to_string(blocks.size()) + " blocks)");
  return blocks[h];
}

BBIDX IRICFG::exceptionOf(BBIDX h) const { return exceptionEdge(blocks, h); }

std::vector<BBIDX> IRICFG::reversePostOrder(EdgeMask mask) const {
  return reversePostOrder(entry, mask);
}

std::vector<BBIDX> IRICFG::reversePostOrder(BBIDX start, EdgeMask mask) const {
  std::vector<BBIDX> post;
  std::vector<char> seen(blocks.size(), 0);
  std::function<void(BBIDX)> dfs = [&](BBIDX u) {
    seen[u] = 1;
    for (Edge e : at(u).succs)
      if ((mask & edgeMask(e.kind)) && !seen[e.to])
        dfs(e.to);
    post.push_back(u);
  };
  dfs(start);
  std::reverse(post.begin(), post.end());
  return post;
}

// ---------------------------------------------------------------------------
// Write-back
// ---------------------------------------------------------------------------

void IRICFG::commit(const std::vector<BBIDX> &order) {
  if (order.size() != blocks.size())
    throw std::runtime_error("commit: order does not cover every block");
  std::vector<char> seen(blocks.size(), 0);
  for (BBIDX h : order) {
    if (h >= blocks.size() || seen[h])
      throw std::runtime_error(
          "commit: order is not a permutation of the blocks");
    seen[h] = 1;
  }

  BBContainerSupport bbc(container, ctx);
  IRID newBBList = ListSEXP::create(ctx, ctx.storage.strings.intern("BB"));
  bbc.setArg_BB(newBBList);

  std::vector<IRID> newBBS;
  newBBS.reserve(order.size());
  for (BBIDX h : order) {
    const IRIBB &bb = at(h);
    // IDX = handle, and nothing is translated: the graph's numbering is the
    // artifact's numbering. That is what deleted globalIDs.
    IRID bbID = BBSEXP::create(ctx, false, false, false, false, false, bb.SCOPE,
                               h, NO_BB);
    // EXCIDX is consumed by construction; the artifact does not carry it.
    BBSEXP(bbID, ctx).clearEXCIDX();

    // The artifact has no body/terminal split, so the terminal is re-appended.
    std::vector<IRID> stmtList = bb.stmts;
    stmtList.push_back(bb.terminal);
    ctx.storage.nodes.set_args(bbID, stmtList);
    newBBS.push_back(bbID);
  }
  ctx.storage.nodes.set_args(newBBList, newBBS);
}

// ---------------------------------------------------------------------------
// Debug names
// ---------------------------------------------------------------------------

std::string IRICFG::debugID() const {
  return "c" + std::to_string(BBContainerSEXP(container, ctx).getStartBBIDX());
}

std::string IRICFG::debugName(BBIDX h) const {
  if (h == entry)
    return "Entry";
  if (h == exit)
    return "Exit";
  return debugID() + "/bb" + std::to_string(h);
}

std::string IRICFG::debugStmtID(BBIDX h, IRID stmt) const {
  const IRIBB &bb = at(h);
  auto it = std::find(bb.stmts.begin(), bb.stmts.end(), stmt);
  if (it == bb.stmts.end())
    return debugName(h) + "/term";
  return debugName(h) + "/i" +
         std::to_string(std::distance(bb.stmts.begin(), it));
}

// ---------------------------------------------------------------------------
// BBEditor
// ---------------------------------------------------------------------------

BBEditor::BBEditor(IRICFG &cfg, BBIDX block)
    : cfg(cfg), block(block), body(cfg.at(block).stmts) {}

BBEditor::~BBEditor() {
  if (dirty)
    cfg.blocks[block].stmts = std::move(body);
}

size_t BBEditor::find(IRID anchor) const {
  auto it = std::find(body.begin(), body.end(), anchor);
  if (it == body.end())
    throw std::runtime_error(
        "BBEditor: anchor is not a statement of this block");
  return std::distance(body.begin(), it);
}

void BBEditor::check(IRID stmt) const {
  IRI_META meta = get_meta(IRI_NODE(cfg.ctx, stmt).tag);
  if (meta != IRI_META::STMT && meta != IRI_META::AMP)
    throw std::runtime_error("BBEditor: not a statement");
  if (isControlFlow(IRI_NODE(cfg.ctx, stmt).tag))
    throw std::runtime_error("BBEditor: control flow belongs to setTerminal");
  if (std::find(body.begin(), body.end(), stmt) != body.end())
    throw std::runtime_error("BBEditor: statement is already placed");
}

void BBEditor::insertBefore(IRID anchor, IRID stmt) {
  check(stmt);
  body.insert(body.begin() + find(anchor), stmt);
  dirty = true;
}

void BBEditor::insertAfter(IRID anchor, IRID stmt) {
  check(stmt);
  body.insert(body.begin() + find(anchor) + 1, stmt);
  dirty = true;
}

void BBEditor::replace(IRID anchor, IRID stmt) {
  check(stmt);
  body[find(anchor)] = stmt;
  dirty = true;
}

void BBEditor::erase(IRID stmt) {
  if (stmt == cfg.blocks[block].terminal)
    throw std::runtime_error(
        "BBEditor::erase: the terminal is not part of the body");
  std::erase(body, stmt);
  dirty = true;
}

// A bit dangerous... will look into this later...
void BBEditor::setTerminal(IRID stmt) {
  if (!isTerminator(IRI_NODE(cfg.ctx, stmt).tag))
    throw std::runtime_error("BBEditor::setTerminal: not a terminator");
  if (std::find(body.begin(), body.end(), stmt) != body.end())
    throw std::runtime_error(
        "BBEditor::setTerminal: statement is already in the body");

  cfg.blocks[block].terminal = stmt;
  deriveEdges(cfg.blocks, block, stmt, Shape{cfg.entry, cfg.exit, cfg.ctx});
  dirty = true;
}

void BBEditor::setException(BBIDX target) {
  setExceptionEdge(cfg.blocks, block, target,
                   Shape{cfg.entry, cfg.exit, cfg.ctx});
  dirty = true;
}

} // namespace IRI_STRUCTURAL
