#include "Generated/IridiumTypes.h"
#include "Support/ClosureTree.hpp"
#include "Support/PTA.hpp"
#include "Support/PTA/PTACallHelpers.hpp"
#include "Support/PTA/PTAContext.hpp"
#include "external/Prakriti.hpp"
#include <memory>
#include <set>
#include <vector>

namespace IRI_STRUCTURAL {

/**
 * AST Tag: Lambda
 * Meta:    RVAL
 * Arguments: (none)
 * Flags:
 *   - string NAME -> sexp.getNAME()
 *   - bool   CNAME -> sexp.getCNAME()
 *   - bool   SETNAME -> sexp.getSETNAME()
 *   - double StartBBIDX -> sexp.getStartBBIDX()
 */
void computeLambdaVals(const PTAStatementContext &ptactx, IRID node,
                      std::set<Prakriti::NodeUID> &res_) {
  Prakriti::ECMAGraph *G = ptactx.incomingState;

  if (!G->hasNode(node)) {
    IRI_GEN::LambdaSEXP sexp(node, ptactx.ctx);
    IRICFG *calleeCFG =
        ptactx.ctx.closureTree->getClosureByStartBBIDX(sexp.getStartBBIDX());
    IRIContext *ctxPtr = &ptactx.ctx;

    auto action = std::make_shared<Prakriti::ECMAGraph::ActionClosureImpl>(
        [ctxPtr, calleeCFG, node](const Prakriti::ECMAGraph::PJSSL_ARG &args)
            -> Prakriti::ECMAGraph::PJSSL_RET {
          std::set<Prakriti::NodeUID> thisVal;
          std::vector<std::set<Prakriti::NodeUID>> positional;
          if (args.A.size() >= 1) {
            auto thisSlice = decodeArgRanges(args.L, args.A[0]);
            if (!thisSlice.empty())
              thisVal = thisSlice[0];
          }
          if (args.A.size() >= 2)
            positional = decodeArgRanges(args.L, args.A[1]);

          auto retVals = PTASolver::invokeClosure(*ctxPtr, calleeCFG, node,
                                                  args.G, thisVal, positional);

          Prakriti::ECMAGraph::PJSSL_RET ret;
          ret.L = std::vector<Prakriti::NodeUID>(retVals.begin(), retVals.end());
          return ret;
        });

    Prakriti::AllocClosure(G, node, action, Prakriti::PKRGlobalState::getTRUE(),
                           Prakriti::PKRGlobalState::getGFOBJ_Function_prototype());
  }

  res_.insert(node);
}

} // namespace IRI_STRUCTURAL
