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

// This feels annoying because for Array Iterator obkjects in Prakriti we were
// explciitly storing PKR_ITERATED, this hacky code will have to do for now
// sadly...
struct IterStep {
  Prakriti::NodeUID fn;
  Prakriti::NodeUID self;
};

// Get iterators, throw if any has "return" ~ its rare so skip it for now
std::vector<IterStep> getIteratorObjects(Prakriti::ECMAGraph *G,
                                         Prakriti::NodeUID iterable) {
  auto itFns = getProperty(G, {iterable}, PKR_SYMBOL_LABEL(iterator));
  auto iterFns = callablesOf({itFns.begin(), itFns.end()});
  if (iterFns.empty())
    throw std::runtime_error(
        "PTA: iterating a value with no callable [[Symbol.iterator]] is not "
        "modelled");

  std::vector<Prakriti::NodeUID> iterators;
  std::vector<Prakriti::ECMAGraph> branches;
  auto itCall = Prakriti::makeCall({iterable}, {});
  Prakriti::Karma(G, iterFns, {nullptr, itCall.L, itCall.A}, iterators,
                  branches);
  Prakriti::KarmaJoin(G, branches);

  std::vector<IterStep> steps;
  for (Prakriti::NodeUID it :
       std::set<Prakriti::NodeUID>(iterators.begin(), iterators.end())) {
    // TODO: We currently dont model return() ...
    auto rets = getProperty(G, {it}, "return");
    if (!callablesOf({rets.begin(), rets.end()}).empty())
      throw std::runtime_error(
          "PTA: an iterator with a return() method is not modelled "
          "(IteratorClose would run user code)");

    auto nexts = getProperty(G, {it}, "next");
    for (Prakriti::NodeUID n : callablesOf({nexts.begin(), nexts.end()}))
      steps.push_back({n, it});
  }
  if (steps.empty())
    throw std::runtime_error(
        "PTA: [[Symbol.iterator]] returned no object with a callable next()");
  return steps;
}

// One step of every iterator, giving back the {value, done} result objects.
void stepIterators(Prakriti::ECMAGraph *G, const std::vector<IterStep> &steps,
                   std::vector<Prakriti::NodeUID> &results,
                   std::vector<Prakriti::ECMAGraph> &branches) {
  for (const auto &step : steps) {
    auto call = Prakriti::makeCall({step.self}, {});
    Prakriti::Karma(G, {step.fn}, {nullptr, call.L, call.A}, results, branches);
  }
}

// Get iterators, call next() once..
void forOfStep(Prakriti::ECMAGraph *G,
               const std::set<Prakriti::NodeUID> &iterables,
               std::set<Prakriti::NodeUID> &values,
               std::set<Prakriti::NodeUID> &dones) {
  for (Prakriti::NodeUID obj : iterables) {
    auto steps = getIteratorObjects(G, obj);

    std::vector<Prakriti::NodeUID> results;
    std::vector<Prakriti::ECMAGraph> branches;
    stepIterators(G, steps, results, branches);
    Prakriti::KarmaJoin(G, branches);

    std::set<Prakriti::NodeUID> res(results.begin(), results.end());
    auto v = getProperty(G, res, "value");
    auto d = getProperty(G, res, "done");
    values.insert(v.begin(), v.end());
    dones.insert(d.begin(), d.end());
  }
}

void appendSpread(Prakriti::ECMAGraph *G,
                  const std::set<Prakriti::NodeUID> &targets,
                  const std::set<Prakriti::NodeUID> &spreads) {
  for (Prakriti::NodeUID s : spreads) {
    auto steps = getIteratorObjects(G, s);

    // A spread has no loop of its own, so this call accounts for the whole
    // iteration: step until the graph stops changing. Karma only ever unions,
    // so it terminates.
    while (true) {
      Prakriti::ECMAGraph before = G->clone();

      std::vector<Prakriti::NodeUID> results;
      std::vector<Prakriti::ECMAGraph> stepBranches;
      stepIterators(G, steps, results, stepBranches);
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
  } else if (IRI_NODE(ctx, rval).tag == IRI_GEN::JSForOfNext) {
    // Case 2
    IRI_GEN::JSForOfNextSEXP jfn(rval, ctx);
    // hasAWAIT only says the flag is present; getAWAIT is the value.
    if (jfn.hasAWAIT() && jfn.getAWAIT())
      throw std::runtime_error(
          "PTA unhandled case CompoundAssn over JSForOfNext [AWAIT]: async "
          "iteration needs the deferred execution model");

    std::set<Prakriti::NodeUID> iterables;
    {
      Prakriti::TraceHelperAuto tho("JSForOfNext::Obj", jfn.getArg_Obj());
      resolvePKRRVal(ptactx, jfn.getArg_Obj(), iterables);
    }
    assert(!iterables.empty());

    std::set<Prakriti::NodeUID> values, dones;
    {
      Prakriti::TraceHelperAuto ths("JSForOfNext::Step", rval);
      forOfStep(G, iterables, values, dones);
    }

    stackWrite(G, ctx, args[1], dones);
    stackWrite(G, ctx, args[2], values);
  } else if (IRI_NODE(ctx, rval).tag == IRI_GEN::JSForInNext) {
    // Case 3 - Trivial
    IRI_GEN::JSForInNextSEXP jfn(rval, ctx);

    std::set<Prakriti::NodeUID> iterators;
    {
      Prakriti::TraceHelperAuto thi("JSForInNext::IteratorObj",
                                    jfn.getArg_IteratorObj());
      resolvePKRRVal(ptactx, jfn.getArg_IteratorObj(), iterators);
    }
    assert(!iterators.empty());

    stackWrite(G, ctx, args[1],
               {Prakriti::PKRGlobalState::getTRUE(),
                Prakriti::PKRGlobalState::getFALSE()});
    stackWrite(G, ctx, args[2], {Prakriti::PKRGlobalState::getSTRING()});
  } else {
    throw std::runtime_error("PTA unhandled case CompoundAssn over " +
                             IRI_GEN::dump_tag(IRI_NODE(ctx, rval).tag));
  }
}

} // namespace IRI_STRUCTURAL
