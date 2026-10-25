#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>
#include <stdexcept>
#include <vector>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: CallSite
 * Meta:    AMP
 * Arguments: (none)
 * Flags:
 *   - void   CCall -> sexp.hasCCall()
 *   - void   ConstructorCall -> sexp.hasConstructorCall()
 *   - void   PrivateCall -> sexp.hasPrivateCall()
 *   - void   Import -> sexp.hasImport()
 *   - void   Super -> sexp.hasSuper()
 *   - void   V8Intrinsic -> sexp.hasV8Intrinsic()
 *   - void   TAILCALL -> sexp.hasTAILCALL()
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
void computeCallSiteVals(const PTAStatementContext &ptactx, IRID node,
                         std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("CallSiteRVal", node);

  IRI_GEN::CallSiteSEXP sexp(node, ptactx.ctx);
  if (sexp.hasPrivateCall() || sexp.hasImport() || sexp.hasSuper() ||
      sexp.hasV8Intrinsic())
    throw std::runtime_error(
        "PTA CallSite: this calling convention is not yet implemented");

  Prakriti::ECMAGraph *G = ptactx.incomingState;
  auto rawArgs = ptactx.ctx.storage.nodes.get_args(node);
  assert(!rawArgs.empty());

  std::set<Prakriti::NodeUID> thisVal;
  size_t calleeIdx = 0;
  if (sexp.hasCCall()) {
    Prakriti::TraceHelperAuto th("CallSiteRVal::This", rawArgs[0]);
    resolvePKRRVal(ptactx, rawArgs[0], thisVal);
    assert(!thisVal.empty());
    calleeIdx = 1;
  }

  std::set<Prakriti::NodeUID> callees;
  {
    Prakriti::TraceHelperAuto th("CallSiteRVal::Callee", rawArgs[calleeIdx]);
    resolvePKRRVal(ptactx, rawArgs[calleeIdx], callees);
  }
  assert(!callees.empty());
  std::vector<Prakriti::NodeUID> calleeVec(callees.begin(), callees.end());

  if (sexp.hasConstructorCall()) {
    // Allocation site abstraction, reciever is created here
    // CallSite IRID itself becomes the allocation site abstraction :)
    std::vector<Prakriti::NodeUID> protos;
    {
      Prakriti::TraceHelperAuto th("CallSiteRVal::GetPrototype",
                                   rawArgs[calleeIdx]);
      std::vector<Prakriti::ECMAGraph> branches;
      for (const auto callee : calleeVec) {
        auto getClosures = G->getPointees(
            callee, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
        Prakriti::Karma(G, getClosures,
                        {nullptr, {callee, callee}, {"prototype"}}, protos,
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
      Prakriti::TraceHelperAuto th("CallSiteRVal::Alloc", node);
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
    Prakriti::TraceHelperAuto th("CallSiteRVal::ArgList", node);
    for (size_t i = calleeIdx + 1; i < rawArgs.size(); i++) {
      std::set<Prakriti::NodeUID> a;
      resolvePKRRVal(ptactx, rawArgs[i], a);
      assert(!a.empty());
      positional.push_back(a);
    }
  }

  std::vector<Prakriti::NodeUID> valVec;
  {
    Prakriti::TraceHelperAuto th("CallSiteRVal::Call", rawArgs[calleeIdx]);
    auto call = Prakriti::makeCall(thisVal, positional);
    std::vector<Prakriti::ECMAGraph> callBranches;
    Prakriti::Karma(G, calleeVec, {nullptr, call.L, call.A}, valVec,
                    callBranches);
    Prakriti::KarmaJoin(G, callBranches);
  }
  std::set<Prakriti::NodeUID> vals(valVec.begin(), valVec.end());

  if (!sexp.hasConstructorCall()) {
    res_.insert(vals.begin(), vals.end());
    return;
  }

  // ECMA [[Construct]]: an explicit object return value wins, else `this`.
  std::set<Prakriti::NodeUID> objs;
  for (const auto v : vals)
    if (Prakriti::isObjectNode(G, v))
      objs.insert(v);
  const auto &result = objs.empty() ? thisVal : objs;
  res_.insert(result.begin(), result.end());
}

} // namespace IRI_STRUCTURAL
