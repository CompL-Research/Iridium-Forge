#include "Support/PTA.hpp"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Storage/IRIContext.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/ClosureTree.hpp"
#include "Support/IRICFG.hpp"
#include "Support/PTA/PTADispatch.hpp"
#include "external/Prakriti.hpp"
#include "external/pta_trace.hpp"
#include <algorithm>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace IRI_STRUCTURAL {

using IRI_STORAGE::IRIContext;
using Prakriti::ECMAGraph;

ECMAGraph PTATransfer::transferStatement(const IRIStatement &stmt,
                                         const ECMAGraph &incomingState) {
  auto nextState = incomingState.clone();
  dispatchPTAStatement(stmt, &nextState, stmt.bb->ctx);
  return nextState;
}

ECMAGraph PTATransfer::transferEdge(BBIDX from, BBIDX to,
                                    const ECMAGraph &exitState) {
  return exitState;
}

namespace {

std::string nodeLabel(IRIContext &ctx, Prakriti::NodeUID node) {
  auto tag = IRI_NODE(ctx, node).tag;
  if (tag == IRI_GEN::EnvBinding) {
    return std::string(
        ctx.storage.strings.get(EnvBindingSEXP(node, ctx).getNAME()));
  }
  if (tag == IRI_GEN::ScriptBinding) {
    return std::string(
        ctx.storage.strings.get(ScriptBindingSEXP(node, ctx).getNAME()));
  }
  return dump_tag(tag);
}

// Wires up ptf::TraceWriter so a viewer can step through the analysis.
void setupTraceWriter(IRIContext &ctx) {
  ptf::Options opt;
  opt.path = "out.pta";
  opt.producer_name = "IRI-PTASolver";
  opt.producer_version = "1.0";
  opt.module_name = "IRI_PTA_ART";

  ctx.debugger.traceWriter = std::make_shared<ptf::TraceWriter>(opt);
  ctx.debugger.traceNodeMetaMapper = [&ctx](Prakriti::NodeUID node) {
    std::unordered_map<std::string, std::string> res;
    auto globalName = Prakriti::PKRGlobalState::getNodeName(node);
    res["label"] = globalName.has_value() ? *globalName : nodeLabel(ctx, node);
    return res;
  };

  ctx.closureTree->preorderTraversal([&](IRICFG *cfg) {
    auto &c = ctx.debugger.traceWriter->declareClosure(cfg->getDebugID(),
                                                       cfg->getDebugName());
    for (const BBIDX bbID : cfg->getReversePostOrder(true)) {
      IRIBB *bb = IRI_BB(cfg, bbID);
      auto &currBB = c.block(bb->getDebugID(), bb->getDebugName());

      for (IRIStatement *s = bb->head; s != nullptr; s = s->next) {
        std::stringstream text;
        IRI_NODE(ctx, s->id).dumpFlat(text, &ctx);
        currBB.inst(s->getDebugID(), text.str())
            .op(dump_tag(IRI_NODE(ctx, s->id).tag));
      }
      if (bb->tail) {
        std::stringstream text;
        IRI_NODE(ctx, bb->tail->id).dumpFlat(text, &ctx);
        currBB.inst(bb->tail->getDebugID(), text.str());
      }

      for (const auto p : cfg->predecessors[bbID])
        currBB.pred(IRI_BB(cfg, p)->getDebugID());
      for (const auto s : cfg->successors[bbID])
        currBB.succ(IRI_BB(cfg, s)->getDebugID());
    }
  });
  ctx.debugger.traceWriter->writeClosures();
}

void initGlobalBindings(IRIContext &ctx, IRICFG *rootCFG, ECMAGraph *G,
                        const Prakriti::GlobalInitTable &globals) {
  for (const auto ref : rootCFG->ptaAssertGlobals()) {
    GlobalBindingSEXP gb(ref, ctx);
    Prakriti::initNamedGlobal(G, gb.getNAME(), globals);
  }
}

std::vector<Prakriti::NodeUID> initStackLocals(IRICFG *rootCFG, ECMAGraph *G) {
  std::vector<Prakriti::NodeUID> locals;
  for (const auto b : rootCFG->ptaGenStack()) {
    if (!G->hasNode(b))
      Prakriti::AllocStackObject(G, b);
    locals.push_back(b);
  }
  for (const auto b : rootCFG->ptaGenTStack()) {
    if (!G->hasNode(b))
      Prakriti::AllocTransientStackObject(G, b);
    locals.push_back(b);
  }
  return locals;
}

void pruneToReachable(ECMAGraph *G,
                      const std::vector<Prakriti::NodeUID> &locals,
                      const std::vector<Prakriti::NodeUID> &remoteRefs) {
  std::vector<Prakriti::NodeUID> roots = {
      Prakriti::PKRGlobalState::getINF(),
      Prakriti::PKRGlobalState::getNAN(),
      Prakriti::PKRGlobalState::getUNDEF(),
      Prakriti::PKRGlobalState::getNULL(),
      Prakriti::PKRGlobalState::getTRUE(),
      Prakriti::PKRGlobalState::getFALSE(),
      Prakriti::PKRGlobalState::getNUMBER(),
      Prakriti::PKRGlobalState::getSTRING(),
      Prakriti::PKRGlobalState::getBIGINT(),
      Prakriti::PKRGlobalState::getGOOBJ_Object_prototype(),
      Prakriti::PKRGlobalState::getGFOBJ_Function_prototype(),
      Prakriti::PKRGlobalState::getGOOBJ_Array_prototype(),
  };
  for (const auto name : Prakriti::PKRGlobalState::getGlobals())
    roots.push_back(Prakriti::PKRGlobalState::getGlobal(name));
  roots.insert(roots.end(), locals.begin(), locals.end());
  roots.insert(roots.end(), remoteRefs.begin(), remoteRefs.end());
  G->pruneUnreachable(roots);
}

// Opens the strong-update window on the closure's own captured cells. Returns
// the cells this activation opened: a recursive activation finds them already
// open, drops to weak updates, and leaves the unwind to the frame that owns it.
std::vector<Prakriti::NodeUID> openTransientCells(IRICFG *cfg, ECMAGraph *G) {
  std::vector<Prakriti::NodeUID> owned;
  for (const auto b : cfg->ptaGenTStack()) {
    if (TransientCell::isExecuting(G, b)) {
      TransientCell::setStrong(G, b, false);
      continue;
    }
    TransientCell::park(G, b);
    TransientCell::setStrong(G, b, true);
    TransientCell::setExecuting(G, b, true);
    owned.push_back(b);
  }
  return owned;
}

void closeTransientCells(ECMAGraph *G,
                         const std::vector<Prakriti::NodeUID> &owned) {
  for (const auto b : owned) {
    TransientCell::close(G, b);
    G->removeAllOutgoingEdgesByLabel(
        b, TransientCell::label(PKR_TRANSIENCE_BACKUP));
    TransientCell::setExecuting(G, b, false);
  }
}

// Asserts the closure's captured references exist, and closes the window on any
// whose owning frame is still live: this body is about to read a cell that
// frame has been strongly updating, so its parked values have to come back
// before the read.
std::vector<Prakriti::NodeUID> bindRemoteRefs(IRICFG *cfg, ECMAGraph *G) {
  auto refs = cfg->ptaGenRemoteRefs();
  for (const auto ref : refs) {
    if (!G->hasNode(ref))
      throw std::runtime_error("Remote binding not found!");
    if (TransientCell::isExecuting(G, ref))
      TransientCell::close(G, ref);
  }
  return refs;
}

std::vector<Prakriti::NodeUID>
setupFileFrame(IRIContext &ctx, IRICFG *rootCFG, ECMAGraph *G,
               const Prakriti::GlobalInitTable &globals) {
  initGlobalBindings(ctx, rootCFG, G, globals);

  auto remoteRefs = bindRemoteRefs(rootCFG, G);

  auto locals = initStackLocals(rootCFG, G);
  pruneToReachable(G, locals, remoteRefs);
  return openTransientCells(rootCFG, G);
}

// Runs the file's entry closure to a dataflow fixpoint.
void runFile(IRIContext &ctx, IRICFG *rootCFG, ECMAGraph *G,
            const Prakriti::GlobalInitTable &globals) {
  auto owned = setupFileFrame(ctx, rootCFG, G, globals);

  PTATransfer transferFunction;
  YADataflowSolver<ECMAGraph> solver(transferFunction);
  solver.run(
      ResumableDataflowState<ECMAGraph>(rootCFG, rootCFG->entry_block, *G),
      true);

  closeTransientCells(G, owned);
}

// Allocate Stack bindings, JSCTX, bind arguments, etc.. some assertions like
// captured existing...
std::vector<Prakriti::NodeUID> setupClosureFrame(
    IRIContext &ctx, IRICFG *calleeCFG, ECMAGraph *G,
    const std::set<Prakriti::NodeUID> &thisVal,
    const std::vector<std::set<Prakriti::NodeUID>> &actualArgs) {
  std::vector<Prakriti::NodeUID> locals;
  for (const auto b : calleeCFG->ptaGenStack()) {
    if (!G->hasNode(b))
      Prakriti::AllocStackObject(G, b);
    locals.push_back(b);
  }
  for (const auto b : calleeCFG->ptaGenTStack()) {
    if (!G->hasNode(b))
      Prakriti::AllocTransientStackObject(G, b);
    locals.push_back(b);
  }

  auto owned = openTransientCells(calleeCFG, G);
  bindRemoteRefs(calleeCFG, G);

  std::vector<std::pair<double, Prakriti::NodeUID>> formals;
  for (const auto b : locals) {
    IRI_GEN::EnvBindingSEXP eb(b, ctx);
    if (eb.hasJSARG())
      formals.push_back({eb.getREFIDX(), b});
  }
  std::sort(formals.begin(), formals.end());

  for (size_t i = 0; i < formals.size(); i++) {
    Prakriti::NodeUID param = formals[i].second;
    // First arg is the ctx... I keep forgetting that, keeping this here just
    // for reference...
    std::vector<Prakriti::NodeUID> setArgs = {param};
    if (i < actualArgs.size())
      setArgs.insert(setArgs.end(), actualArgs[i].begin(), actualArgs[i].end());
    else
      setArgs.push_back(Prakriti::PKRGlobalState::getUNDEF());
    auto setClosures =
        G->getPointees(param, Prakriti::PKRGlobalState::EdgeIntern(PKR_Set));
    Prakriti::KarmaBindu(G, setClosures, {nullptr, setArgs});
  }

  auto sentinel = [&](int slot) {
    return Prakriti::PKRGlobalState::generateSentinel(
        calleeCFG->id,
        Prakriti::PKRGlobalState::EdgeIntern("JSCTX" + std::to_string(slot)));
  };

  Prakriti::NodeUID argsID = sentinel(0);
  if (!G->hasNode(argsID))
    Prakriti::AllocArgumentsObject(
        G, argsID, Prakriti::PKRGlobalState::getTRUE(),
        Prakriti::PKRGlobalState::getGOOBJ_Object_prototype());

  Prakriti::NodeUID margsID = sentinel(1);
  if (!G->hasNode(margsID)) {
    Prakriti::AllocMappedArgumentsObject(
        G, margsID, Prakriti::PKRGlobalState::getTRUE(),
        Prakriti::PKRGlobalState::getGOOBJ_Object_prototype());
    for (const auto &[refidx, param] : formals)
      G->addEdge(
          margsID, param,
          Prakriti::PKRGlobalState::EdgeIntern(std::to_string((long)refidx)));
  }

  // TODO: Added WIPStackObject Nodes, will need to implement these later...
  for (int slot = 2; slot <= 8; slot++) {
    Prakriti::NodeUID id = sentinel(slot);
    if (!G->hasNode(id))
      Prakriti::AllocWIPStackObject(G, id);
  }

  Prakriti::NodeUID thisID = sentinel(9);
  if (!G->hasNode(thisID))
    Prakriti::AllocStackObject(G, thisID);
  {
    std::vector<Prakriti::NodeUID> setArgs = {thisID};
    if (!thisVal.empty())
      setArgs.insert(setArgs.end(), thisVal.begin(), thisVal.end());
    else
      setArgs.push_back(Prakriti::PKRGlobalState::getUNDEF());
    auto setClosures =
        G->getPointees(thisID, Prakriti::PKRGlobalState::EdgeIntern(PKR_Set));
    Prakriti::KarmaBindu(G, setClosures, {nullptr, setArgs});
  }

  return owned;
}

using StackSnapshot =
    std::unordered_map<Prakriti::NodeUID, std::vector<Prakriti::NodeUID>>;

// Uncaptured cells are unreachable outside their frame, so they stay strong
// throughout and an activation's writes would clobber what a still-live outer
// activation of the same closure holds. Snapshot on entry, put back on exit.
// Captured cells use the parked-backup window instead, since a closure may
// still need what this activation left behind.
StackSnapshot snapshotStackCells(IRICFG *calleeCFG, ECMAGraph *G) {
  StackSnapshot snapshot;
  for (const auto b : calleeCFG->ptaGenStack())
    if (G->hasNode(b))
      snapshot[b] =
          G->getPointees(b, Prakriti::PKRGlobalState::EdgeIntern(PKR_STK));
  return snapshot;
}

void restoreStackCells(IRICFG *calleeCFG, ECMAGraph *G,
                       const StackSnapshot &snapshot) {
  for (const auto b : calleeCFG->ptaGenStack()) {
    if (!G->hasNode(b))
      continue;
    G->removeAllOutgoingEdgesByLabel(
        b, Prakriti::PKRGlobalState::EdgeIntern(PKR_STK));
    auto it = snapshot.find(b);
    if (it == snapshot.end())
      continue;
    for (const auto v : it->second)
      G->addEdge(b, v, Prakriti::PKRGlobalState::EdgeIntern(PKR_STK));
  }
}

// Calling [[Get]] on the <common-ret>
std::set<Prakriti::NodeUID> readReturnValue(IRICFG *calleeCFG,
                                            ECMAGraph *finalState) {
  auto getClosures = finalState->getPointees(
      calleeCFG->retCTX, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
  return Prakriti::KarmaBindu(finalState, getClosures,
                              {nullptr, {calleeCFG->retCTX}});
}

std::pair<std::set<Prakriti::NodeUID>, ECMAGraph>
extractReturnValue(IRICFG *calleeCFG,
                   std::unordered_map<IRIStatement *, ECMAGraph> &states) {
  IRIStatement *tailStmt = calleeCFG->get_bb(calleeCFG->exit_block)->tail;
  auto it = states.find(tailStmt);
  if (it == states.end())
    throw std::runtime_error("PTA: closure exit block unreachable");

  ECMAGraph finalState = it->second;
  auto vals = readReturnValue(calleeCFG, &finalState);
  return {vals, finalState};
}

} // namespace

void PTASolver::solve(IRIContext &ctx) {
  Prakriti::setTraceEnabled(ctx.flags.dumpPTA);
  setupTraceWriter(ctx);

  Prakriti::PKRGlobalState::Init(
      [&ctx]() { return PTAStubSEXP::create(ctx); },
      [&ctx](std::string_view s) { return ctx.storage.strings.intern(s); },
      [&ctx](Prakriti::EdgeUID e) { return ctx.storage.strings.get(e); });

  finRes.clear();

  IRICFG *rootCFG = ctx.closureTree->getRoot();
  ECMAGraph currState;

  using ActionClosureTarget =
      std::function<ECMAGraph::PJSSL_RET(ECMAGraph::PJSSL_ARG)>;

  auto env = Prakriti::QJSScriptFile();
  env.alloc(&currState, rootCFG->id,
           std::make_shared<ActionClosureTarget>([&](ECMAGraph::PJSSL_ARG arg) {
             runFile(ctx, rootCFG, arg.G, env.globals);
             return ECMAGraph::PJSSL_RET();
           }));

  // Repeatedly drain pending Eval-like actions off JSFILE nodes until the
  // set of live files stops changing.
  bool done = false;
  while (!done) {
    std::vector<ECMAGraph> results;
    for (const auto n : currState.getAllNodes()) {
      if (currState.getNodeTAG(n) != Prakriti::TAG::JSFILE)
        continue;
      ECMAGraph G = currState.clone();
      auto acts = G.getPointees(n, ctx.storage.strings.intern(PKR_Eval));
      Prakriti::KarmaBindu(&G, acts, {NULL, {n}});
      G.removeNode(n);
      results.push_back(G);
    }
    ECMAGraph nextState;
    nextState.mutateMergeUnion(results);

    done = currState == nextState;
    currState = nextState;
  }

  ctx.debugger.traceWriter->finish();
  Prakriti::setTraceEnabled(false);
}

std::set<Prakriti::NodeUID> PTASolver::invokeClosure(
    IRIContext &ctx, IRICFG *calleeCFG, Prakriti::NodeUID closureID,
    ECMAGraph *G, const std::set<Prakriti::NodeUID> &thisVal,
    const std::vector<std::set<Prakriti::NodeUID>> &actualArgs) {
  auto snapshot = snapshotStackCells(calleeCFG, G);
  auto owned = setupClosureFrame(ctx, calleeCFG, G, thisVal, actualArgs);

  // Recursion guard, for each closure we store a vector of tuples
  // (code, value_ctx|null). If the closure is recursively reached again with
  // the same value_ctx we return, if we dont we will end up in infinite loops.
  auto stateHash = G->hash();
  auto &entries = progressTracker[closureID];
  for (auto &[hash, result] : entries) {
    if (hash != stateHash)
      continue;
    if (!result.has_value()) {
      closeTransientCells(G, owned);
      restoreStackCells(calleeCFG, G, snapshot);
      return {};
    }
    ECMAGraph finalState = *result;
    auto retVals = readReturnValue(calleeCFG, &finalState);
    closeTransientCells(&finalState, owned);
    restoreStackCells(calleeCFG, &finalState, snapshot);
    *G = finalState;
    return retVals;
  }

  entries.push_back({stateHash, std::nullopt});
  size_t idx = entries.size() - 1;

  PTATransfer transferFunction;
  YADataflowSolver<ECMAGraph> solver(transferFunction);
  auto cfgAtStmt = solver.run(
      ResumableDataflowState<ECMAGraph>(calleeCFG, calleeCFG->entry_block, *G),
      true);

  auto [retVals, finalState] = extractReturnValue(calleeCFG, cfgAtStmt);
  progressTracker[closureID][idx] = {stateHash, finalState};

  closeTransientCells(&finalState, owned);
  restoreStackCells(calleeCFG, &finalState, snapshot);
  *G = finalState;
  return retVals;
}

} // namespace IRI_STRUCTURAL
