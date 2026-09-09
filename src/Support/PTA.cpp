#include "Support/PTA.hpp"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Storage/IridiumPool.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/ClosureTree.hpp"
#include "Support/IRICFG.hpp"
#include "Support/IRIS.hpp"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTADispatch.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "external/Prakriti.hpp"
#include "external/pta_trace.hpp"
#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
namespace IRI_STRUCTURAL {

using IRI_STORAGE::IridiumPool;
using Prakriti::ECMAGraph;

ECMAGraph PTATransfer::transferStatement(const IRIStatement &stmt,
                                         const ECMAGraph &incomingState) {
  auto nextState = incomingState.clone();
  dispatchPTAStatement(stmt, &nextState, stmt.bb->pool);
  return nextState;
}

ECMAGraph PTATransfer::transferEdge(BBIDX from, BBIDX to,
                                    const ECMAGraph &exitState) {
  return exitState;
}

void PTASolver::solve(IridiumPool &pool) {

  ptf::Options opt;
  opt.path = "out.pta";
  opt.producer_name = "IRI-PTASolver";
  opt.producer_version = "1.0";
  opt.module_name = "IRI_PTA_ART";

  std::function<std::string(Prakriti::NodeUID)> getIRINodeLabel =
      [&](Prakriti::NodeUID node) {
        auto currTAG = pool[node].tag;
        if (currTAG == IRI_GEN::EnvBinding) {
          EnvBindingSEXP eb(node, pool);
          return std::string(pool.strings.get(eb.getNAME()));
        } else {
          return dump_tag(currTAG);
        }
      };

  pool.traceWriter = std::make_shared<ptf::TraceWriter>(opt);
  pool.traceNodeMetaMapper = [&](Prakriti::NodeUID node) {
    std::unordered_map<std::string, std::string> res;
    auto isGlobalName = Prakriti::PKRGlobalState::getNodeName(node);
    std::string label =
        isGlobalName.has_value() ? *isGlobalName : getIRINodeLabel(node);
    res["label"] = label;
    return res;
  };

  // 1. Declare program structure (may also be done after analysis).
  pool.closureTree->preorderTraversal([&](IRICFG *cfg) {
    auto &c = pool.traceWriter->declareClosure(cfg->getDebugID(),
                                               cfg->getDebugName());
    auto bbs = cfg->getReversePostOrder(true);

    // I am currently localizing inst number to each closure
    size_t instID = 0;
    for (const BBIDX bbID : bbs) {
      IRIBB *bb = (*cfg)[bbID];
      auto &currBB = c.block(bb->getDebugID(), bb->getDebugName());
      for (IRIStatement *s = bb->head; s != nullptr; s = s->next) {
        std::stringstream instTextStream;
        auto &inst = pool[s->id];
        inst.dumpFlat(instTextStream, &pool);

        auto &i = currBB.inst(s->getDebugID(), instTextStream.str());

        i.op(dump_tag(inst.tag));
      }
      if (bb->tail) {
        std::stringstream instTextStream;
        pool[bb->tail->id].dumpFlat(instTextStream, &pool);
        auto &i = currBB.inst(bb->tail->getDebugID(), instTextStream.str());
      }
      for (const auto p : cfg->predecessors[bbID]) {
        currBB.pred((*cfg)[p]->getDebugID());
      }

      for (const auto s : cfg->successors[bbID]) {
        currBB.succ((*cfg)[s]->getDebugID());
      }
    }
  });
  pool.traceWriter->writeClosures();
  PTATransfer transferFunction;

  std::function<Prakriti::NodeUID()> reserveNodeUID = [&]() {
    return PTAStubSEXP::create(pool);
  };

  std::function<Prakriti::NodeUID(std::string_view)> edgeIntern =
      [&pool](std::string_view s) { return pool.strings.intern(s); };

  std::function<std::string_view(Prakriti::EdgeUID)> edgeGet =
      [&pool](Prakriti::EdgeUID e) { return pool.strings.get(e); };

  Prakriti::PKRGlobalState::Init(reserveNodeUID, edgeIntern, edgeGet);
  // Prakriti::PKRGlobalState::DumpDebugInfo(std::cout);

  finRes.clear();

  IRICFG *rootCFG = pool.closureTree->getRoot();
  ECMAGraph currState;

  using ActionClosureTarget =
      std::function<ECMAGraph::PJSSL_RET(ECMAGraph::PJSSL_ARG)>;

  Prakriti::AllocECMAScriptFile(
      &currState, rootCFG->id,
      std::make_shared<ActionClosureTarget>([&](ECMAGraph::PJSSL_ARG arg) {
        ECMAGraph *G = arg.G;

        // ASSERT -- references to global env
        std::vector<IRID> globalBindings = rootCFG->ptaAssertGlobals();

        for (const auto ref : globalBindings) {
          GlobalBindingSEXP gb(ref, pool);

          Prakriti::PKRGlobalState::getGlobal(gb.getNAME());
        }

        // ASSERT -- references to remote stack nodes
        auto remoteRefs = rootCFG->ptaAssertTransient();
        for (const auto ref : remoteRefs) {
          if (!G->hasNode(ref)) {
            throw std::runtime_error("Remote binding not found!");
          }
        }

        // Generate local stack nodes
        auto locals = rootCFG->ptaGenStack();
        for (const auto b : locals) {
          if (!G->hasNode(b)) {
            Prakriti::AllocStackObject(G, b);
          }
        }

        // Generate and initialize lifetime of transient stack nodes
        auto cLocals = rootCFG->ptaGenTStack();
        for (const auto b : cLocals) {
          throw std::runtime_error("TODO:: handle transient stack nodes!");
        }

        // Call the Dataflow solver on the file
        YADataflowSolver<ECMAGraph> solver(transferFunction);
        solver.run(ResumableDataflowState<ECMAGraph>(rootCFG,
                                                     rootCFG->entry_block, *G),
                   true);

        std::cout << "YADataflowSolver Done" << std::endl;
        // Handle exit merge of Transient Stack Nodes
        // for (const auto b : cLocals) {
        //   throw std::runtime_error("TODO:: handle transient stack nodes!");
        // }

        return ECMAGraph::PJSSL_RET();
      }));

  bool done = false;
  while (done == false) {
    std::vector<ECMAGraph> res;
    std::vector<Prakriti::NodeUID> processedNodes;
    auto nodes = currState.getAllNodes();
    size_t innerIter = 0;
    for (auto n : nodes) {
      if (currState.getNodeTAG(n) == Prakriti::TAG::JSFILE) {
        ECMAGraph G = currState.clone();
        auto acts = G.getPointees(n, pool.strings.intern(PKR_Eval));
        Prakriti::KarmaBindu(&G, acts, {NULL, {n}});
        G.removeNode(n);
        res.push_back(G);
      }
    }
    ECMAGraph nextState;
    nextState.mutateMergeUnion(res);

    done = currState == nextState;
    currState = nextState;
  }

  pool.traceWriter->finish();
}

} // namespace IRI_STRUCTURAL
