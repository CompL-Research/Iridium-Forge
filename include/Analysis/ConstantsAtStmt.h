#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumTypes.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/BBContainerSupport.hpp"
#include "Support/IRIS.hpp"
#include <cassert>
#include <iostream>
#include <memory>

namespace IRI_STRUCTURAL {

// ============================================================================
// 1. Constants At Stmt Lattice Definition
// ============================================================================
struct CASLattice {
  enum Kind { TOP, CONST, NAC };

  Kind kind = TOP;
  IRID value = 0;
  IRI_STORAGE::IRIContext *ctx = nullptr;

  CASLattice() = default;
  CASLattice(Kind k, IRID val = 0, IRI_STORAGE::IRIContext *p = nullptr)
      : kind(k), value(val), ctx(p) {}

  static CASLattice bottom() { return CASLattice(NAC); }
  static CASLattice top() { return CASLattice(TOP); }

  bool constantsEqual(IRID a, IRID b) const {
    if (a == b)
      return true;
    IRI_STORAGE::IRIContext *p = ctx;
    if (!p)
      return false;
    auto tagA = p->storage.nodes.get_node(a).tag;
    auto tagB = p->storage.nodes.get_node(b).tag;
    if (tagA != tagB)
      return false;
    switch (tagA) {
    case IRI_GEN::Number: {
      IRI_GEN::NumberSEXP nA(a, *p);
      IRI_GEN::NumberSEXP nB(b, *p);
      return nA.getIridiumPrimitive() == nB.getIridiumPrimitive();
    }
    case IRI_GEN::Boolean: {
      IRI_GEN::BooleanSEXP bA(a, *p);
      IRI_GEN::BooleanSEXP bB(b, *p);
      return bA.getIridiumPrimitive() == bB.getIridiumPrimitive();
    }
    case IRI_GEN::Null:
      return true;
    case IRI_GEN::JSNUBD:
      return true;
    case IRI_GEN::String: {
      IRI_GEN::StringSEXP sA(a, *p);
      IRI_GEN::StringSEXP sB(b, *p);
      return sA.getIridiumPrimitive() == sB.getIridiumPrimitive();
    }
    case IRI_GEN::JSBigInt: {
      IRI_GEN::JSBigIntSEXP iA(a, *p);
      IRI_GEN::JSBigIntSEXP iB(b, *p);
      return iA.getIridiumPrimitive() == iB.getIridiumPrimitive();
    }
    default:
      return false;
    }
  }

  // Lattice join operator (Least Upper Bound ⊔)
  [[nodiscard]] CASLattice joinWith(const CASLattice &other) const {
    IRI_STORAGE::IRIContext *p = ctx ? ctx : other.ctx;
    if (kind == TOP) {
      return CASLattice(other.kind, other.value, p);
    }
    if (other.kind == TOP) {
      return CASLattice(kind, value, p);
    }
    if (kind == NAC || other.kind == NAC) {
      return CASLattice(NAC, 0, p);
    }
    // Both are CONST
    if (constantsEqual(value, other.value)) {
      return CASLattice(CONST, value, p);
    }
    return CASLattice(NAC, 0, p);
  }

  bool operator==(const CASLattice &other) const {
    if (kind != other.kind)
      return false;
    if (kind == CONST) {
      IRI_STORAGE::IRIContext *p = ctx ? ctx : other.ctx;
      if (p) {
        CASLattice temp = *this;
        temp.ctx = p;
        return temp.constantsEqual(value, other.value);
      }
      return value == other.value;
    }
    return true;
  }
};

// ============================================================================
// 2. Constants At Stmt Dataflow State
// ============================================================================
class CASState {
private:
  bool is_unreachable = true;
  ImmutableDataMap<IRID, CASLattice> envBindings;

public:
  // Default constructor creates an unreachable/unvisited state
  CASState() : is_unreachable(true) {}
  explicit CASState(ImmutableDataMap<IRID, CASLattice> bindings,
                    bool unreachable = false)
      : envBindings(std::move(bindings)), is_unreachable(unreachable) {}

  // Solver-required interface: Bottom of the join-semilattice represents
  // unvisited/unreachable
  static CASState bottom() {
    return CASState(); // Default constructor creates unreachable state
  }

  static CASState reachableEmpty() {
    return CASState(ImmutableDataMap<IRID, CASLattice>(), false);
  }

  bool isUnreachable() const { return is_unreachable; }

  void dump(IRI_STORAGE::IRIContext &ctx, std::ostream &oss) const {
    if (is_unreachable) {
      oss << "<Unreachable>";
      return;
    }
    oss << "{";
    bool first = true;
    for (const auto &[bID, lattice] : envBindings.getRawMap()) {
      if (!first)
        oss << ", ";
      first = false;
      if (IRI_NODE(ctx, bID).tag == IRI_GEN::EnvBinding) {
        IRI_GEN::EnvBindingSEXP eb(bID, ctx);
        oss << ctx.storage.strings.get(eb.getNAME());
      } else {
        oss << "IRID(" << bID << ")";
      }
      oss << ": ";
      if (lattice.kind == CASLattice::TOP) {
        oss << "TOP";
      } else if (lattice.kind == CASLattice::CONST) {
        oss << "CONST(";
        auto cID = lattice.value;
        auto tag = IRI_NODE(ctx, cID).tag;
        if (tag == IRI_GEN::Number) {
          oss << IRI_GEN::NumberSEXP(cID, ctx).getIridiumPrimitive();
        } else if (tag == IRI_GEN::Boolean) {
          oss << (IRI_GEN::BooleanSEXP(cID, ctx).getIridiumPrimitive()
                      ? "true"
                      : "false");
        } else if (tag == IRI_GEN::Null) {
          oss << "null";
        } else if (tag == IRI_GEN::JSNUBD) {
          oss << "undefined";
        } else if (tag == IRI_GEN::String) {
          oss << "\""
              << ctx.storage.strings.get(
                     IRI_GEN::StringSEXP(cID, ctx).getIridiumPrimitive())
              << "\"";
        } else if (tag == IRI_GEN::JSBigInt) {
          oss << ctx.storage.strings.get(
                     IRI_GEN::JSBigIntSEXP(cID, ctx).getIridiumPrimitive())
              << "n";
        } else {
          oss << "IRID(" << cID << ")";
        }
        oss << ")";
      } else if (lattice.kind == CASLattice::NAC) {
        oss << "NAC";
      }
    }
    oss << "}";
  }

  // Lattice join for states
  [[nodiscard]] CASState joinWith(const CASState &other) const {
    if (is_unreachable)
      return other;
    if (other.is_unreachable)
      return *this;
    if (*this == other)
      return *this;

    return CASState(envBindings.joinWith(other.envBindings), false);
  }

  bool operator==(const CASState &other) const {
    if (is_unreachable != other.is_unreachable)
      return false;
    if (is_unreachable)
      return true;
    return envBindings == other.envBindings;
  }

  // Get Lattice value for a binding
  CASLattice getLattice(IRID binding) const {
    return envBindings.get(binding, CASLattice::top());
  }

  // Immutable update: returns a new state with binding mapped to value
  CASState setLattice(IRID binding, CASLattice val) const {
    return CASState(envBindings.set(binding, val), false);
  }
};

// ============================================================================
// 3. Constants At Stmt Transfer Function
// ============================================================================
class ConstantsAtStmtTransfer : public TransferFunction<CASState> {
public:
  CASState transferStatement(const IRIStatement &stmt,
                             const CASState &incomingState) override {
    if (incomingState.isUnreachable()) {
      return CASState::bottom();
    }

    CASState nextState = incomingState;
    auto &ctx = stmt.bb->ctx;
    auto tag = IRI_NODE(ctx, stmt.id).tag;

    if (tag == IRI_GEN::LWrite) {
      nextState = processWrite(stmt.id, nextState, ctx);
    } else if (tag == IRI_GEN::CompoundAssn) {
      auto args = ctx.storage.nodes.get_args(stmt.id);
      assert(args.size() > 1);
      for (size_t i = 1; i < args.size(); i++) {
        IRID currWriteID = args[i];
        if (IRI_NODE(ctx, currWriteID).tag == IRI_GEN::LWrite) {
          IRI_GEN::LWriteSEXP lw(currWriteID, ctx);
          auto lval = lw.getArg_LValTarget();
          if (IRI_NODE(ctx, lval).tag == IRI_GEN::EnvBinding) {
            nextState = nextState.setLattice(
                lval, CASLattice(CASLattice::NAC, 0, &ctx));
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
      auto & iris = ctx.iris;
      if (IRI_BINDING(iris, lval).isCaptured() || IRI_BINDING(iris, lval).isEvalTainted()) {
        return state;
      }
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
      return CASLattice(CASLattice::CONST, node, &ctx);
    }
    return CASLattice(CASLattice::NAC, 0, &ctx);
  }
};

// ============================================================================
// 4. Constants At Stmt Analysis Result and Pass Definition
// ============================================================================
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

  void dumpStateAtStatement(const IRIStatement &stmt,
                            IRI_STORAGE::IRIContext &ctx,
                            std::ostream &os) const override {
    CASState state = queryStateAtStatement(stmt);
    state.dump(ctx, os);
  }

  void dumpBlockEntryState(BBIDX block, IRI_STORAGE::IRIContext &ctx,
                           std::ostream &os) const override {
    const CASState &state = getBlockEntryState(block);
    state.dump(ctx, os);
  }

  void dumpBlockExitState(BBIDX block, IRI_STORAGE::IRIContext &ctx,
                          std::ostream &os) const override {
    os << "<ExitStateNotTracked>";
  }

  const CASState &getBlockEntryState(BBIDX block) const {
    return solver->getBlockEntryState(block);
  }

  CASState queryStateAtStatement(const IRIStatement &stmt) const {
    return solver->queryStateAtStatement(stmt);
  }
};

struct ConstantsAtStmt {
  static inline char ID = 0;
  using Result = ConstantsAtStmtResult;

  ConstantsAtStmtResult run(IRICFG &cfg, AnalysisManager &am) {
    auto transfer = std::make_shared<ConstantsAtStmtTransfer>();
    auto solver = std::make_shared<DataflowSolver<CASState>>(cfg, *transfer);

    // Initial State
    double headScope = cfg.nodeMap.at(cfg.entry_block)->SCOPE;
    auto bindings = cfg.ctx.iris->getEnvBindingsInClosure(headScope);
    CASState entryState = CASState::reachableEmpty();
    auto &iris = cfg.ctx.iris;
    for (auto bID : bindings) {
      bool isArg = false;
      if (IRI_NODE(cfg.ctx, bID).tag == IRI_GEN::EnvBinding) {
        IRI_GEN::EnvBindingSEXP eb(bID, cfg.ctx);
        isArg = eb.hasJSARG() || eb.hasJSRESTARG();
      }
      entryState = entryState.setLattice(bID, (IRI_BINDING(iris, bID).isCaptured() || isArg)
                                                   ? CASLattice::bottom()
                                                   : CASLattice::top());
    }

    solver->run(entryState, /*includeExceptions=*/true);

    return ConstantsAtStmtResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
