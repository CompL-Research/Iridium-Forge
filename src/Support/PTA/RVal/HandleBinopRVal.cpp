#include "Generated/IridiumTypes.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "Support/PTA/PTARVALHandlers.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>
#include <stdexcept>
#include <string>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Binop
 * Meta:    AMP
 * Arguments:
 *   [0] IRID LBinop -> sexp.getArg_LBinop()
 *   [1] IRID RBinop -> sexp.getArg_RBinop()
 * Flags:
 *   - string OP -> sexp.getOP()
 */
void computeBinopVals(const PTAStatementContext &ptactx, IRID node,
                      std::set<Prakriti::NodeUID> &res_) {
  IRI_GEN::BinopSEXP sexp(node, ptactx.ctx);
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  std::string op(ptactx.ctx.storage.strings.get(sexp.getOP()));

  //
  // JSBinops: "==" | "===" | "!=" | "!==" | "in" | "instanceof" | "|>";
  //
  // Traditional Binops have been mapped to the following in Prakriti:
  //   1. NAC_HandleBinop: "**", "*",  "/", "%", "+", "-", "<<", ">>", ">>>",
  //   "&", "^", "|"
  //   2. NAC_HandleRelop: "<", ">", "<=", ">="
  //
  static const std::set<std::string> arithBitwiseOps = {
      "**", "*", "/", "%", "+", "-", "<<", ">>", ">>>", "&", "^", "|"};
  static const std::set<std::string> relopOps = {"<", ">", "<=", ">="};

  Prakriti::ECMAGraph::ActionClosure handlerClosure;
  if (arithBitwiseOps.count(op)) {
    handlerClosure = Prakriti::PKRGlobalState::NAC_HandleBinop;
  } else if (relopOps.count(op)) {
    handlerClosure = Prakriti::PKRGlobalState::NAC_HandleRelop;
  } else {
    throw std::runtime_error("PTA RVal TODO: unhandled Binop operator '" + op +
                             "'");
  }

  std::set<Prakriti::NodeUID> lvals, rvals;
  resolvePKRRVal(ptactx, sexp.getArg_LBinop(), lvals);
  resolvePKRRVal(ptactx, sexp.getArg_RBinop(), rvals);
  assert(!lvals.empty() && !rvals.empty());

  Prakriti::NodeUID binopAct =
      Prakriti::PKRGlobalState::getActionNode(handlerClosure);

  for (Prakriti::NodeUID lval : lvals) {
    for (Prakriti::NodeUID rval : rvals) {
      auto ret = Prakriti::invokeAction(binopAct, {G, {lval, rval}, {op}});
      res_.insert(ret.L.begin(), ret.L.end());
    }
  }
}

} // namespace IRI_STRUCTURAL
