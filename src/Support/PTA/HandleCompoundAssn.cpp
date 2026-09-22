#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Storage/IRIContext.h"
#include "Support/PTA/PTAContext.hpp"
#include "Support/PTA/PTAHandlers.hpp"
#include "Support/PTA/PTAObjectHelpers.hpp"
#include "Support/PTA/PTARVALDispatch.hpp"
#include "external/Prakriti.hpp"
#include <cassert>
#include <set>
#include <stdexcept>
#include <vector>

namespace IRI_STRUCTURAL {

namespace {

void appendSpread(Prakriti::ECMAGraph *G,
                  const std::set<Prakriti::NodeUID> &targets,
                  const std::set<Prakriti::NodeUID> &spreads) {
  for (Prakriti::NodeUID s : spreads) {
    // Get the [%Symbol.iterator%] methods
    auto itFns = getProperty(G, {s}, PKR_SYMBOL_LABEL(iterator));
    auto iterFns = callablesOf({itFns.begin(), itFns.end()});
    if (iterFns.empty())
      throw std::runtime_error(
          "PTA: spreading a value with no callable [[Symbol.iterator]] is not "
          "modelled");

    // Get all the { next: ()=>{}, ... } objects
    std::vector<Prakriti::NodeUID> iterators;
    std::vector<Prakriti::ECMAGraph> branches;
    auto itCall = Prakriti::makeCall({s}, {});
    Prakriti::Karma(G, iterFns, {nullptr, itCall.L, itCall.A}, iterators,
                    branches);
    Prakriti::KarmaJoin(G, branches);

    // Collect all next() methods
    std::vector<Prakriti::NodeUID> steppers;
    std::vector<Prakriti::NodeUID> receivers;
    for (Prakriti::NodeUID it :
         std::set<Prakriti::NodeUID>(iterators.begin(), iterators.end())) {
      auto nexts = getProperty(G, {it}, "next");
      for (Prakriti::NodeUID n : callablesOf({nexts.begin(), nexts.end()})) {
        steppers.push_back(n);
        receivers.push_back(it);
      }
    }
    if (steppers.empty())
      throw std::runtime_error(
          "PTA: [[Symbol.iterator]] returned no object with a callable next()");

    // call next() until fixed point
    while (true) {
      Prakriti::ECMAGraph before = G->clone();

      std::vector<Prakriti::NodeUID> results;
      std::vector<Prakriti::ECMAGraph> stepBranches;
      for (size_t i = 0; i < steppers.size(); i++) {
        auto call = Prakriti::makeCall({receivers[i]}, {});
        Prakriti::Karma(G, {steppers[i]}, {nullptr, call.L, call.A}, results,
                        stepBranches);
      }
      Prakriti::KarmaJoin(G, stepBranches);

      auto yielded = getProperty(G, {results.begin(), results.end()}, "value");
      definePropertyValue(G, targets, PKR_UNKNOWN_FIELD, yielded);

      if (G->equals(before))
        break;
    }
  }
}

} // namespace

/**
 * AST Tag: CompoundAssn
 * Meta:    STMT
 * Arguments: (none)
 * Flags: (none)
 */
void handleCompoundAssn(const PTAStatementContext &ptactx) {
  Prakriti::TraceHelperAuto th("CompoundAssn", ptactx.stmt.id);

  auto &ctx = ptactx.ctx;
  Prakriti::ECMAGraph *G = ptactx.incomingState;
  auto args = ctx.storage.nodes.get_args(ptactx.stmt.id);

  if (args.size() != 3)
    throw std::runtime_error("PTA unhandled case CompoundAssn with " +
                             std::to_string(args.size() - 1) + " destinations");

  IRID rval = args[0];
  if (IRI_NODE(ctx, rval).tag == IRI_GEN::JSAppend) {
    // Case 1
    IRI_GEN::JSAppendSEXP jsa(rval, ctx);
    std::set<Prakriti::NodeUID> targets, spreads, indices;
    {
      Prakriti::TraceHelperAuto tht("JSAppend::TargetObj",
                                    jsa.getArg_TargetObj());
      resolvePKRRVal(ptactx, jsa.getArg_TargetObj(), targets);
    }
    {
      Prakriti::TraceHelperAuto ths("JSAppend::SpreadObj",
                                    jsa.getArg_SpreadObj());
      resolvePKRRVal(ptactx, jsa.getArg_SpreadObj(), spreads);
    }
    {
      Prakriti::TraceHelperAuto thi("JSAppend::InsertionIdx",
                                    jsa.getArg_InsertionIdx());
      resolvePKRRVal(ptactx, jsa.getArg_InsertionIdx(), indices);
    }
    assert(!targets.empty());
    assert(!spreads.empty());

    {
      Prakriti::TraceHelperAuto tha("JSAppend::Append", rval);
      appendSpread(G, targets, spreads);
    }

    stackWrite(G, ctx, args[1], {Prakriti::PKRGlobalState::getNUMBER()});
    stackWrite(G, ctx, args[2], targets);
  } else {
    // Case 2
    throw std::runtime_error("PTA unhandled case CompoundAssn over " +
                             IRI_GEN::dump_tag(IRI_NODE(ctx, rval).tag));
  }
}

} // namespace IRI_STRUCTURAL
