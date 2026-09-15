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
#include <memory>
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

// Named globals (globalThis, undefined, Function, ...) each need a specific
// Prakriti init routine; this table replaces initializing them one at a time
// with a hardcoded if/else chain on the interned name.
const std::vector<std::pair<const char *, void (*)(ECMAGraph *)>> &
globalInitializers() {
  static const std::vector<std::pair<const char *, void (*)(ECMAGraph *)>>
      table = {
          {GSTK_globalThis, Prakriti::initGSTK_globalThis},
          {GSTK_Infinity, Prakriti::initGSTK_Infinity},
          {GSTK_NaN, Prakriti::initGSTK_NaN},
          {GSTK_undefined, Prakriti::initGSTK_undefined},
          {GSTK_Function, Prakriti::initGSTK_Function},
          {GSTK_Boolean, Prakriti::initGSTK_Boolean},
          {GSTK_Symbol, Prakriti::initGSTK_Symbol},
          {GSTK_Error, Prakriti::initGSTK_Error},
          {GSTK_Object, Prakriti::initGSTK_Object},
      };
  return table;
}

void initGlobalBindings(IRIContext &ctx, IRICFG *rootCFG, ECMAGraph *G) {
  for (const auto ref : rootCFG->ptaAssertGlobals()) {
    GlobalBindingSEXP gb(ref, ctx);
    bool matched = false;
    for (const auto &[name, initFn] : globalInitializers()) {
      if (gb.getNAME() == Prakriti::PKRGlobalState::EdgeIntern(name)) {
        initFn(G);
        matched = true;
        break;
      }
    }
    if (!matched)
      throw std::runtime_error("Unknown Global");
  }
}

std::vector<Prakriti::NodeUID> initStackLocals(IRICFG *rootCFG,
                                                ECMAGraph *G) {
  std::vector<Prakriti::NodeUID> locals;
  for (const auto b : rootCFG->ptaGenStack()) {
    if (!G->hasNode(b))
      Prakriti::AllocStackObject(G, b);
    locals.push_back(b);
  }
  // Top-level captured locals can always be strong-updated, no transience
  // needed since it always holds at this scope.
  for (const auto b : rootCFG->ptaGenTStack()) {
    if (!G->hasNode(b))
      Prakriti::AllocStackObject(G, b);
    locals.push_back(b);
  }
  return locals;
}

void pruneToReachable(ECMAGraph *G, const std::vector<Prakriti::NodeUID> &locals,
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

// Runs the file's entry closure to a dataflow fixpoint, seeding it with
// global bindings, stack locals, and any remote (already-bound) references.
void runFile(IRIContext &ctx, IRICFG *rootCFG, ECMAGraph *G) {
  initGlobalBindings(ctx, rootCFG, G);

  auto remoteRefs = rootCFG->ptaAssertTransient();
  for (const auto ref : remoteRefs) {
    if (!G->hasNode(ref))
      throw std::runtime_error("Remote binding not found!");
  }

  auto locals = initStackLocals(rootCFG, G);
  pruneToReachable(G, locals, remoteRefs);

  PTATransfer transferFunction;
  YADataflowSolver<ECMAGraph> solver(transferFunction);
  solver.run(
      ResumableDataflowState<ECMAGraph>(rootCFG, rootCFG->entry_block, *G),
      true);
}

} // namespace

void PTASolver::solve(IRIContext &ctx) {
  // The dataflow solver (YADataflowSolver::run) always calls into
  // ctx.debugger.traceWriter -- it isn't optional debug scaffolding, so it
  // has to exist regardless of dumpPTA. dumpPTA only controls the extra
  // per-event detail gated by Prakriti::isTraceEnabled().
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

  Prakriti::AllocECMAScriptFile(
      &currState, rootCFG->id,
      std::make_shared<ActionClosureTarget>([&](ECMAGraph::PJSSL_ARG arg) {
        runFile(ctx, rootCFG, arg.G);
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

} // namespace IRI_STRUCTURAL
