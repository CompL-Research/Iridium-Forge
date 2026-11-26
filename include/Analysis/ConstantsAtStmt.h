#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/BBContainerSupport.hpp"
#include "Support/IRIS.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

namespace IRI_STRUCTURAL {

//
// Lattice
//
struct CASLattice {
  enum Kind { TOP, CONST, NAC };
  Kind kind = TOP;
  IRID value = 0;

  CASLattice(Kind k, IRID val = 0) : kind(k), value(val) {}

  [[nodiscard]] CASLattice joinWith(const CASLattice &other) const {
    if (kind == TOP)
      return other;
    if (other.kind == TOP)
      return *this;
    if (kind == NAC || other.kind == NAC)
      return CASLattice(NAC);
    return constantsEqual(value, other.value) ? *this : CASLattice(NAC);
  }

  bool operator==(const CASLattice &other) const {
    if (kind != other.kind)
      return false;
    if (kind != CONST)
      return true;
    return constantsEqual(value, other.value);
  }

  static inline IRI_STORAGE::IRIContext *ctx = nullptr;
  static bool constantsEqual(IRID a, IRID b) {
    if (a == b)
      return true;
    auto tagA = ctx->storage.nodes.get_node(a).tag;
    auto tagB = ctx->storage.nodes.get_node(b).tag;
    if (tagA != tagB)
      return false;
    switch (tagA) {
    case IRI_GEN::Number: {
      IRI_GEN::NumberSEXP nA(a, *ctx);
      IRI_GEN::NumberSEXP nB(b, *ctx);
      return nA.getIridiumPrimitive() == nB.getIridiumPrimitive();
    }
    case IRI_GEN::Boolean: {
      IRI_GEN::BooleanSEXP bA(a, *ctx);
      IRI_GEN::BooleanSEXP bB(b, *ctx);
      return bA.getIridiumPrimitive() == bB.getIridiumPrimitive();
    }
    case IRI_GEN::Null:
      return true;
    case IRI_GEN::JSNUBD:
      return true;
    case IRI_GEN::String: {
      IRI_GEN::StringSEXP sA(a, *ctx);
      IRI_GEN::StringSEXP sB(b, *ctx);
      return sA.getIridiumPrimitive() == sB.getIridiumPrimitive();
    }
    case IRI_GEN::JSBigInt: {
      IRI_GEN::JSBigIntSEXP iA(a, *ctx);
      IRI_GEN::JSBigIntSEXP iB(b, *ctx);
      return iA.getIridiumPrimitive() == iB.getIridiumPrimitive();
    }
    default:
      return false;
    }
  }
};

//
// Lattice = Lattice X Bindings
//
class CASState {
private:
  ImmutableDataMap<IRID, CASLattice> envBindings;

public:
  CASState() = default;
  explicit CASState(ImmutableDataMap<IRID, CASLattice> bindings)
      : envBindings(std::move(bindings)) {}

  static CASState over(const std::vector<IRID> &bindings,
                       const CASLattice &initial) {
    ImmutableDataMap<IRID, CASLattice> map;
    for (IRID bID : bindings)
      map = map.set(bID, initial);
    return CASState(map);
  }

  [[nodiscard]] CASState joinWith(const CASState &other) const {
    if (*this == other)
      return *this;

    return CASState(envBindings.joinWith(other.envBindings));
  }

  bool operator==(const CASState &other) const {
    return envBindings == other.envBindings;
  }

  CASLattice getLattice(IRID binding) const { return envBindings.get(binding); }

  CASState setLattice(IRID binding, CASLattice val) const {
    assert(envBindings.contains(binding));
    return CASState(envBindings.set(binding, val));
  }
};

//
// Transfer Function
//
class ConstantsAtStmtTransfer : public TransferFunction<CASState> {
public:
  CASState transferStatement(IRIContext &ctx, IRID stmt,
                             const CASState &incomingState) override {
    CASState nextState = incomingState;
    auto tag = IRI_NODE(ctx, stmt).tag;

    if (tag == IRI_GEN::LWrite) {
      nextState = processWrite(stmt, nextState, ctx);
    } else if (tag == IRI_GEN::CompoundAssn) {
      auto args = ctx.storage.nodes.get_args(stmt);
      assert(args.size() > 1);
      for (size_t i = 1; i < args.size(); i++) {
        IRID currWriteID = args[i];
        if (IRI_NODE(ctx, currWriteID).tag == IRI_GEN::LWrite) {
          IRI_GEN::LWriteSEXP lw(currWriteID, ctx);
          auto lval = lw.getArg_LValTarget();
          if (IRI_NODE(ctx, lval).tag == IRI_GEN::EnvBinding) {
            nextState = nextState.setLattice(lval, CASLattice(CASLattice::NAC));
          }
        }
      }
    }

    return nextState;
  }

  CASState transferEdge(BBIDX from, BBIDX to,
                        const CASState &exitState) override {
    return exitState;
  }

private:
  CASState processWrite(IRID node, const CASState &state,
                        IRI_STORAGE::IRIContext &ctx) {
    auto tag = IRI_NODE(ctx, node).tag;
    if (tag == IRI_GEN::LWrite) {
      IRI_GEN::LWriteSEXP lw(node, ctx);
      auto lval = lw.getArg_LValTarget();
      auto &iris = ctx.iris;
      if (IRI_BINDING(iris, lval).isCaptured() ||
          IRI_BINDING(iris, lval).isEvalTainted()) {
        return state;
      }
      // A write to a function's own name binding is ignored at runtime
      // (CreateImmutableBinding with Annex B), so it must not forward its
      // value to later reads; the declaration's initializing write does store.
      if (!lw.hasINIT() && IRI_GEN::EnvBindingSEXP(lval, ctx).hasSLOPPYCONST())
        return state;
      if (IRI_NODE(ctx, lval).tag == IRI_GEN::EnvBinding) {
        return state.setLattice(lval,
                                resolveValue(lw.getArg_RVal(), state, ctx));
      }
    }
    return state;
  }

  CASLattice resolveValue(IRID node, const CASState &state,
                          IRI_STORAGE::IRIContext &ctx) {
    auto tag = IRI_NODE(ctx, node).tag;
    if (tag == IRI_GEN::EnvRead) {
      IRI_GEN::EnvReadSEXP er(node, ctx);
      auto target = er.getArg_Obj();
      if (IRI_NODE(ctx, target).tag == IRI_GEN::EnvBinding) {
        return state.getLattice(target);
      }
    }
    // Check if the node itself is a literal/constant node
    if (tag == IRI_GEN::Number || tag == IRI_GEN::Boolean ||
        tag == IRI_GEN::Null || tag == IRI_GEN::String ||
        tag == IRI_GEN::JSBigInt || tag == IRI_GEN::JSNUBD) {
      return CASLattice(CASLattice::CONST, node);
    }
    return CASLattice(CASLattice::NAC);
  }
};

//
// Machinery
//
class ConstantsAtStmtResult : public DataflowResultConcept {
private:
  std::shared_ptr<ConstantsAtStmtTransfer> transfer;
  std::shared_ptr<DataflowSolver<CASState>> solver;

public:
  ConstantsAtStmtResult() = default;
  ConstantsAtStmtResult(std::shared_ptr<ConstantsAtStmtTransfer> t,
                        std::shared_ptr<DataflowSolver<CASState>> s)
      : transfer(std::move(t)), solver(std::move(s)) {}

  std::string getAnalysisName() const override { return "ConstantsAtStmt"; }

  const CASState &getStateBeforeStatement(BBIDX block,
                                          size_t stmtOffset) const {
    return solver->getStateBeforeStatement(block, stmtOffset);
  }
};

struct ConstantsAtStmt {
  static inline char ID = 0;
  using Result = ConstantsAtStmtResult;

  ConstantsAtStmtResult run(IRICFG &cfg, AnalysisManager &am) {
    auto transfer = std::make_shared<ConstantsAtStmtTransfer>();
    auto solver = std::make_shared<DataflowSolver<CASState>>(cfg, *transfer);

    SCPIDX headScope = cfg.at(cfg.entry).SCOPE;
    auto bindings = cfg.ctx.iris->getEnvBindingsInClosure(headScope);
    CASState init = CASState::over(bindings, CASLattice(CASLattice::TOP));

    auto &iris = cfg.ctx.iris;
    CASState entryState = init;
    for (auto bID : bindings) {
      bool isArg = false;
      if (IRI_NODE(cfg.ctx, bID).tag == IRI_GEN::EnvBinding) {
        IRI_GEN::EnvBindingSEXP eb(bID, cfg.ctx);
        isArg = eb.hasJSARG() || eb.hasJSRESTARG();
      }
      if (IRI_BINDING(iris, bID).isCaptured() || isArg)
        entryState = entryState.setLattice(bID, CASLattice(CASLattice::NAC));
    }

    solver->run(entryState, init);

    return ConstantsAtStmtResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
