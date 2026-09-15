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
  if (sexp.hasCCall() || sexp.hasConstructorCall() || sexp.hasPrivateCall() ||
      sexp.hasImport() || sexp.hasSuper() || sexp.hasV8Intrinsic())
    throw std::runtime_error(
        "PTA CallSite: only the basic calling convention is implemented");

  Prakriti::ECMAGraph *G = ptactx.incomingState;
  auto rawArgs = ptactx.ctx.storage.nodes.get_args(node);
  assert(!rawArgs.empty());

  std::set<Prakriti::NodeUID> callees;
  resolvePKRRVal(ptactx, rawArgs[0], callees);
  assert(!callees.empty());

  std::vector<std::set<Prakriti::NodeUID>> positional;
  std::vector<Prakriti::NodeUID> flatArgs;
  for (size_t i = 1; i < rawArgs.size(); i++) {
    std::set<Prakriti::NodeUID> a;
    resolvePKRRVal(ptactx, rawArgs[i], a);
    assert(!a.empty());
    positional.push_back(a);
    flatArgs.insert(flatArgs.end(), a.begin(), a.end());
  }

  std::vector<Prakriti::NodeUID> calleeVec(callees.begin(), callees.end());
  auto vals = Prakriti::KarmaBindu(
      G, calleeVec, {nullptr, flatArgs, {encodeArgRanges(positional)}});
  res_.insert(vals.begin(), vals.end());
}

} // namespace IRI_STRUCTURAL
