#include "Generated/IridiumTypes.h"
#include "Support/ClosureTree.hpp"
#include "Support/PTA.hpp"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
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
  Prakriti::TraceHelperAuto th("LambdaRVal", node);

  Prakriti::ECMAGraph *G = ptactx.incomingState;

  if (!G->hasNode(node)) {
    IRI_GEN::LambdaSEXP sexp(node, ptactx.ctx);
    IRICFG *calleeCFG =
        ptactx.ctx.closureTree->getClosureByStartBBIDX(sexp.getStartBBIDX());

    auto action = std::make_shared<Prakriti::ECMAGraph::ActionClosureImpl>(
        [calleeCFG, node](const Prakriti::ECMAGraph::PJSSL_ARG &args)
            -> Prakriti::ECMAGraph::PJSSL_RET {
          std::set<Prakriti::NodeUID> thisVal;
          std::vector<std::set<Prakriti::NodeUID>> positional;
          if (args.A.size() >= 1) {
            auto thisSlice = Prakriti::decodeArgRanges(args.L, args.A[0]);
            if (!thisSlice.empty())
              thisVal = thisSlice[0];
          }
          if (args.A.size() >= 2)
            positional = Prakriti::decodeArgRanges(args.L, args.A[1]);

          auto retVals = PTASolver::invokeClosure(calleeCFG, node, args.G,
                                                  thisVal, positional);

          Prakriti::ECMAGraph::PJSSL_RET ret;
          ret.L = std::vector<Prakriti::NodeUID>(retVals.begin(), retVals.end());
          return ret;
        });

    {
      Prakriti::TraceHelperAuto th("LambdaRVal::Alloc", node);
      Prakriti::AllocClosure(G, node, action, Prakriti::PKRGlobalState::getTRUE(),
                             Prakriti::PKRGlobalState::getGFOBJ_Function_prototype());
    }

    // 10.2.5 MakeConstructor, for the closures the frontend marked PROTO.
    if (IRI_GEN::BBContainerSEXP(calleeCFG->id, ptactx.ctx).hasPROTO()) {
      Prakriti::TraceHelperAuto th("LambdaRVal::Define", node);

      // A reserved label: sentinels keyed on a plain field name are already
      // taken by that field's FieldProxy.
      Prakriti::NodeUID protoID = Prakriti::PKRGlobalState::generateSentinel(
          node, Prakriti::PKRGlobalState::EdgeIntern("[[FunctionPrototype]]"));
      if (!G->hasNode(protoID))
        Prakriti::AllocOrdinaryObject(
            G, protoID, Prakriti::PKRGlobalState::getTRUE(),
            Prakriti::PKRGlobalState::getGOOBJ_Object_prototype());
      defineProperty(G, {node}, "prototype", {protoID});
      defineProperty(G, {protoID}, "constructor", {node});
    }
  }

  res_.insert(node);
}

} // namespace IRI_STRUCTURAL
