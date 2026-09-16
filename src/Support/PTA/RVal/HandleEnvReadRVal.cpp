#include "Generated/IridiumTypes.h"
#include "Helpers.h"
#include "Support/PTA/PTAContext.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: EnvRead
 * Meta:    AMP
 * Arguments:
 *   [0] IRID Obj -> sexp.getArg_Obj()
 * Flags:
 *   - void   SAFE -> sexp.hasSAFE()
 *   - void   TAINTED -> sexp.hasTAINTED()
 */
void computeEnvReadVals(const PTAStatementContext &ptactx, IRID node,
                        std::set<Prakriti::NodeUID> &res_) {
  IRI_GEN::EnvReadSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  IRID obj = sexp.getArg_Obj();
  auto objTag = IRI_NODE(ptactx.ctx, obj).tag;

  Prakriti::NodeUID target =
      objTag == IRI_GEN::GlobalBinding
          ? Prakriti::PKRGlobalState::getGlobal(
                IRI_GEN::GlobalBindingSEXP(obj, ptactx.ctx).getNAME())
      : objTag == IRI_GEN::RemoteEnvBinding
          ? IRI_HELPERS::resolveRemoteBinding(ptactx.ctx, obj)
          : obj;
  assert(G->hasNode(target));

  auto getClosures =
      G->getPointees(target, Prakriti::PKRGlobalState::EdgeIntern(PKR_Get));
  assert(!getClosures.empty());
  auto vals = Prakriti::KarmaBindu(G, getClosures, {nullptr, {target}});
  res_.insert(vals.begin(), vals.end());
}

} // namespace IRI_STRUCTURAL
