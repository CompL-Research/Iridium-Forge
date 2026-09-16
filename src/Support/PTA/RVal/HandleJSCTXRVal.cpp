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
  IRI_GEN::JSCTXSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  // The the sentinal, we expect this to always exist...
  IRID closureID = ptactx.getBB()->closure->id;
  Prakriti::NodeUID target = Prakriti::PKRGlobalState::generateSentinel(
      closureID, Prakriti::PKRGlobalState::EdgeIntern(
                     "JSCTX" + std::to_string((long)sexp.getOPID())));
  assert(G->hasNode(target));

  auto getClosures =
      G->getPointees(target, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
  assert(!getClosures.empty());
  auto vals = Prakriti::KarmaBindu(G, getClosures, {nullptr, {target}});
  res_.insert(vals.begin(), vals.end());
}

} // namespace IRI_STRUCTURAL
