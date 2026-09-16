#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTACallHelpers.hpp"
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
    resolvePKRRVal(ptactx, rawArgs[0], thisVal);
    assert(!thisVal.empty());
    calleeIdx = 1;
  }

  std::set<Prakriti::NodeUID> callees;
  resolvePKRRVal(ptactx, rawArgs[calleeIdx], callees);
  assert(!callees.empty());
  std::vector<Prakriti::NodeUID> calleeVec(callees.begin(), callees.end());

  if (sexp.hasConstructorCall()) {
    // Allocation site abstraction, reciever is created here
    // CallSite IRID itself becomes the allocation site abstraction :)
    Prakriti::NodeUID thisID = node;
    if (!G->hasNode(thisID))
      Prakriti::AllocOrdinaryObject(G, thisID,
                                    Prakriti::PKRGlobalState::getTRUE(),
                                    Prakriti::PKRGlobalState::getNULL());
    for (const auto callee : calleeVec) {
      auto getClosures =
          G->getPointees(callee, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
      auto protos = Prakriti::KarmaBindu(
          G, getClosures, {nullptr, {callee, callee}, {"prototype"}});
      for (const auto p : protos)
        G->addEdge(thisID, p,
                   Prakriti::PKRGlobalState::EdgeIntern(PKR_PROTOTYPE));
    }
    thisVal = {thisID};
  }

  std::vector<Prakriti::NodeUID> flatArgs(thisVal.begin(), thisVal.end());
  std::string thisRange = thisVal.empty() ? "" : encodeArgRanges({thisVal});

  std::vector<std::set<Prakriti::NodeUID>> positional;
  for (size_t i = calleeIdx + 1; i < rawArgs.size(); i++) {
    std::set<Prakriti::NodeUID> a;
    resolvePKRRVal(ptactx, rawArgs[i], a);
    assert(!a.empty());
    positional.push_back(a);
    flatArgs.insert(flatArgs.end(), a.begin(), a.end());
  }
  std::string argsRange = encodeArgRanges(positional, thisVal.size());

  auto vals = Prakriti::KarmaBindu(G, calleeVec,
                                   {nullptr, flatArgs, {thisRange, argsRange}});

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
