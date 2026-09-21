#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSCTX
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - double OPID -> sexp.getOPID()
 */
void computeJSCTXVals(const PTAStatementContext &ptactx, IRID node,
                      std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("JSCTXRVal", node);

  IRI_GEN::JSCTXSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  // The the sentinal, we expect this to always exist...
  IRID closureID = ptactx.getBB()->closure->id;
  Prakriti::NodeUID target = Prakriti::PKRGlobalState::generateSentinel(
      closureID, Prakriti::PKRGlobalState::EdgeIntern(
                     "JSCTX" + std::to_string((long)sexp.getOPID())));
  assert(G->hasNode(target));

  std::vector<Prakriti::NodeUID> vals;
  {
    Prakriti::TraceHelperAuto th("JSCTXRVal::Get", target);
    auto getClosures =
        G->getPointees(target, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
    assert(!getClosures.empty());
    std::vector<Prakriti::ECMAGraph> branches;
    Prakriti::Karma(G, getClosures, {nullptr, {target}}, vals, branches);
    Prakriti::KarmaJoin(G, branches);
  }
  res_.insert(vals.begin(), vals.end());
}

} // namespace IRI_STRUCTURAL
