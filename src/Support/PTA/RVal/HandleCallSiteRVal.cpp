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
 *   - void   PrivateCall -> sexp.hasPrivateCall()
 *   - void   Import -> sexp.hasImport()
 *   - void   V8Intrinsic -> sexp.hasV8Intrinsic()
 *   - void   TAILCALL -> sexp.hasTAILCALL()
 *   - double JSDirectEval -> sexp.getJSDirectEval()
 */
void computeCallSiteVals(const PTAStatementContext &ptactx, IRID node,
                         std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("CallSiteRVal", node);

  IRI_GEN::CallSiteSEXP sexp(node, ptactx.ctx);
  if (sexp.hasPrivateCall() || sexp.hasImport() || sexp.hasV8Intrinsic())
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

  res_.insert(vals.begin(), vals.end());
}

} // namespace IRI_STRUCTURAL
