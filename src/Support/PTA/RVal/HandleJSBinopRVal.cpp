#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>
#include <string>
#include <vector>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: JSBinop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID LBinop -> sexp.getArg_LBinop()
 *   [1] IRID RBinop -> sexp.getArg_RBinop()
 * Flags:
 *   - string OP -> sexp.getOP()
 */
void computeJSBinopVals(const PTAStatementContext &ptactx, IRID node,
                        std::set<Prakriti::NodeUID> &res_) {
  Prakriti::TraceHelperAuto th("JSBinopRVal", node);

  IRI_GEN::JSBinopSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::string op(ptactx.ctx.storage.strings.get(sexp.getOP()));

  std::set<Prakriti::NodeUID> lvals, rvals;
  {
    Prakriti::TraceHelperAuto th("JSBinopRVal::LBinop", sexp.getArg_LBinop());
    resolvePKRRVal(ptactx, sexp.getArg_LBinop(), lvals);
  }
  {
    Prakriti::TraceHelperAuto th("JSBinopRVal::RBinop", sexp.getArg_RBinop());
    resolvePKRRVal(ptactx, sexp.getArg_RBinop(), rvals);
  }
  assert(!lvals.empty() && !rvals.empty());

  Prakriti::NodeUID act = Prakriti::PKRGlobalState::getActionNode(
      Prakriti::PKRGlobalState::NAC_HandleJSBinop);

  std::vector<Prakriti::NodeUID> vals;
  std::vector<Prakriti::ECMAGraph> branches;
  {
    Prakriti::TraceHelperAuto th("JSBinopRVal::Karma", act);
    for (const auto lval : lvals)
      for (const auto rval : rvals)
        Prakriti::Karma(G, {act}, {nullptr, {lval, rval}, {op}}, vals, branches);
    Prakriti::KarmaJoin(G, branches);
  }

  res_.insert(vals.begin(), vals.end());
}

} // namespace IRI_STRUCTURAL
