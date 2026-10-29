// Generated Stub for IRI_TAG::JSConstruct (RVal)
#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <vector>
#include <set>
#include <stdexcept>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSConstruct
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID NewTarget -> sexp.getArg_NewTarget()
 * Flags: (none)
 */
void computeJSConstructVals(const PTAStatementContext &ptactx, IRID node,
                            std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("JSConstructRVal", node);

  IRI_GEN::JSConstructSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;
  auto rawArgs = ptactx.ctx.storage.nodes.get_args(node);
  assert(rawArgs.size() >= 2);

  std::set<Prakriti::NodeUID> callees;
  {
    Prakriti::TraceHelperAuto th("JSConstructRVal::Callee", rawArgs[0]);
    resolvePKRRVal(ptactx, rawArgs[0], callees);
  }
  assert(!callees.empty());
  std::vector<Prakriti::NodeUID> calleeVec(callees.begin(), callees.end());

  std::set<Prakriti::NodeUID> newTargets;
  {
    Prakriti::TraceHelperAuto th("JSConstructRVal::NewTarget", rawArgs[1]);
    resolvePKRRVal(ptactx, rawArgs[1], newTargets);
  }
  assert(!newTargets.empty());

  // Allocation site abstraction: the new object (this) is created here, and
  // the JSConstruct IRID itself is the allocation site.
  std::set<Prakriti::NodeUID> thisVal;
  {
    // Allocation site abstraction, reciever is created here
    // CallSite IRID itself becomes the allocation site abstraction :)
    std::vector<Prakriti::NodeUID> protos;
    {
      // OrdinaryCreateFromConstructor(newTarget): the prototype comes from
      // new.target, not the callee (they differ for super()).
      Prakriti::TraceHelperAuto th("JSConstructRVal::GetPrototype", rawArgs[1]);
      std::vector<Prakriti::ECMAGraph> branches;
      for (const auto nt : newTargets) {
        auto getClosures = G->getPointees(
            nt, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
        Prakriti::Karma(G, getClosures,
                        {nullptr, {nt, nt}, {"prototype"}}, protos,
                        branches);
      }
      Prakriti::KarmaJoin(G, branches);
    }

    // OrdinaryCreateFromConstructor: a callee whose `prototype` is not an
    // object contributes %Object.prototype% instead.
    std::set<Prakriti::NodeUID> objProtos;
    bool fallback = protos.empty();
    for (const auto p : protos) {
      if (Prakriti::isObjectNode(G, p))
        objProtos.insert(p);
      else
        fallback = true;
    }
    if (fallback)
      objProtos.insert(Prakriti::PKRGlobalState::getGOOBJ_Object_prototype());

    {
      Prakriti::TraceHelperAuto th("JSConstructRVal::Alloc", node);
      bool anyString = false, anyOrdinary = false;
      for (const auto callee : calleeVec) {
        if (callee == Prakriti::PKRGlobalState::getGFOBJ_String())
          anyString = true;
        else
          anyOrdinary = true;
      }

      if (anyString) {
        Prakriti::NodeUID strID = Prakriti::PKRGlobalState::generateSentinel(
            node, Prakriti::PKRGlobalState::EdgeIntern("[[StringObject]]"));
        if (!G->hasNode(strID))
          Prakriti::AllocStringObject(G, strID);
        thisVal.insert(strID);
      }

      if (anyOrdinary) {
        if (!G->hasNode(node))
          Prakriti::AllocOrdinaryObject(G, node,
                                        Prakriti::PKRGlobalState::getTRUE(),
                                        *objProtos.begin());
        for (const auto p : objProtos)
          G->addEdge(node, p, Prakriti::PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
        thisVal.insert(node);
      }
    }
    assert(!thisVal.empty());
  }

  std::vector<std::set<Prakriti::NodeUID>> positional;
  {
    Prakriti::TraceHelperAuto th("JSConstructRVal::ArgList", node);
    for (size_t i = 2; i < rawArgs.size(); i++) {
      std::set<Prakriti::NodeUID> a;
      resolvePKRRVal(ptactx, rawArgs[i], a);
      assert(!a.empty());
      positional.push_back(a);
    }
  }

  std::vector<Prakriti::NodeUID> valVec;
  {
    Prakriti::TraceHelperAuto th("JSConstructRVal::Call", rawArgs[0]);
    auto call = Prakriti::makeCall(thisVal, positional);
    std::vector<Prakriti::ECMAGraph> callBranches;
    Prakriti::Karma(G, calleeVec, {nullptr, call.L, call.A}, valVec,
                    callBranches);
    Prakriti::KarmaJoin(G, callBranches);
  }
  std::set<Prakriti::NodeUID> vals(valVec.begin(), valVec.end());


  // ECMA [[Construct]]: an explicit object return value wins, else `this`.
  std::set<Prakriti::NodeUID> objs;
  for (const auto v : vals)
    if (Prakriti::isObjectNode(G, v))
      objs.insert(v);
  const auto &result = objs.empty() ? thisVal : objs;
  res_.insert(result.begin(), result.end());
}

} // namespace IRI_STRUCTURAL
