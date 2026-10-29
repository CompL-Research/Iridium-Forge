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
 * Arguments:
 *   [0] IRID Callee -> sexp.getArg_Callee()
 *   [1] IRID This -> sexp.getArg_This()
 * Flags:
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
void computeCallSiteVals(const PTAStatementContext &ptactx, IRID node,
                         std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("CallSiteRVal", node);

  IRI_GEN::CallSiteSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;
  auto rawArgs = ptactx.ctx.storage.nodes.get_args(node);
  assert(rawArgs.size() >= 2);

  // [callee, this, args...]. A plain call passes a read of undefined as this,
  // which is left unmodelled (no this value).
  std::set<Prakriti::NodeUID> thisVal;
  const size_t calleeIdx = 0;
  bool plainCall = false;
  if (ptactx.ctx.storage.nodes.get_node(rawArgs[1]).tag == IRI_GEN::EnvRead) {
    IRID b = IRI_GEN::EnvReadSEXP(rawArgs[1], ptactx.ctx).getArg_Obj();
    plainCall = ptactx.ctx.storage.nodes.get_node(b).tag == IRI_GEN::GlobalBinding &&
                ptactx.ctx.storage.strings.get(
                    IRI_GEN::GlobalBindingSEXP(b, ptactx.ctx).getNAME()) == "undefined";
  }
  if (!plainCall) {
    Prakriti::TraceHelperAuto th("CallSiteRVal::This", rawArgs[1]);
    resolvePKRRVal(ptactx, rawArgs[1], thisVal);
    assert(!thisVal.empty());
  }

  std::set<Prakriti::NodeUID> callees;
  {
    Prakriti::TraceHelperAuto th("CallSiteRVal::Callee", rawArgs[calleeIdx]);
    resolvePKRRVal(ptactx, rawArgs[calleeIdx], callees);
  }
  assert(!callees.empty());
  std::vector<Prakriti::NodeUID> calleeVec(callees.begin(), callees.end());


  std::vector<std::set<Prakriti::NodeUID>> positional;
  {
    Prakriti::TraceHelperAuto th("CallSiteRVal::ArgList", node);
    for (size_t i = 2; i < rawArgs.size(); i++) {
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

  res_.insert(vals.begin(), vals.end());
}

} // namespace IRI_STRUCTURAL
