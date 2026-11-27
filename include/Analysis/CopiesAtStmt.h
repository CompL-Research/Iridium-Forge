#pragma once

#include "Analysis/PassManager.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Support/AbstractInterpretation.hpp"
#include "Support/IRIS.hpp"
#include <memory>
#include <vector>

namespace IRI_STRUCTURAL {

//
// Lattice
//
struct CopyLattice {
  enum Kind { UNKNOWN, NOEFFECT, COPY };
  Kind kind = UNKNOWN;
  IRID src = NO_IRID;

  CopyLattice(Kind k) : kind(k) {}
  CopyLattice(Kind k, IRID src) : kind(k), src(src) {}

  [[nodiscard]] CopyLattice joinWith(const CopyLattice &other) const {
    if (kind == UNKNOWN)
      return other;
    if (other.kind == UNKNOWN)
      return *this;
    if (kind == COPY && other.kind == COPY && src == other.src)
      return *this;
    return CopyLattice(NOEFFECT);
  }

  bool operator==(const CopyLattice &other) const {
    return kind == other.kind && src == other.src;
  }
};

//
// Lattice = Lattice X Bindings
//
class CopyState {
private:
  ImmutableDataMap<IRID, CopyLattice> mirrors;

public:
  CopyState() = default;
  explicit CopyState(ImmutableDataMap<IRID, CopyLattice> mirrors)
      : mirrors(std::move(mirrors)) {}

  static CopyState over(const std::vector<IRID> &bindings,
                        const CopyLattice &initial) {
    ImmutableDataMap<IRID, CopyLattice> map;
    for (IRID bID : bindings)
      map = map.set(bID, initial);
    return CopyState(map);
  }

  [[nodiscard]] CopyState joinWith(const CopyState &other) const {
    if (*this == other)
      return *this;

    return CopyState(mirrors.joinWith(other.mirrors));
  }

  bool operator==(const CopyState &other) const {
    return mirrors == other.mirrors;
  }

  bool contains(IRID binding) const { return mirrors.contains(binding); }

  CopyLattice getLattice(IRID binding) const { return mirrors.get(binding); }

  CopyState setLattice(IRID binding, CopyLattice val) const {
    assert(mirrors.contains(binding));
    return CopyState(mirrors.set(binding, val));
  }

  template <typename F> CopyState mapValues(F f) const {
    return CopyState(mirrors.mapValues(f));
  }
};

//
// Transfer Function
//
class CopiesAtStmtTransfer : public TransferFunction<CopyState> {
public:
  CopyState transferStatement(IRIContext &ctx, IRID stmt,
                              const CopyState &incoming) override {
    CopyState next = killWrites(ctx, stmt, incoming);
    if (IRI_NODE(ctx, stmt).tag == IRI_GEN::LWrite)
      next = establish(ctx, stmt, next);
    return next;
  }

private:
  // Only recursive to handle Compound Assn
  static CopyState killWrites(IRIContext &ctx, IRID node,
                              const CopyState &state) {
    CopyState next = state;
    if (IRI_NODE(ctx, node).tag == IRI_GEN::LWrite) {
      IRID target = IRI_GEN::LWriteSEXP(node, ctx).getArg_LValTarget();
      next = next.mapValues([=](IRID key, const CopyLattice &val) {
        if (key == target ||
            (val.kind == CopyLattice::COPY && val.src == target))
          return CopyLattice(CopyLattice::NOEFFECT);
        return val;
      });
    }
    for (IRID child : ctx.storage.nodes.get_args_view(node))
      next = killWrites(ctx, child, next);
    return next;
  }

  // A local write of a local read is the only thing that establishes a copy.
  static CopyState establish(IRIContext &ctx, IRID stmt,
                             const CopyState &state) {
    IRI_GEN::LWriteSEXP lw(stmt, ctx);
    IRID target = lw.getArg_LValTarget();
    IRID rval = lw.getArg_RVal();

    if (IRI_NODE(ctx, rval).tag != IRI_GEN::EnvRead ||
        IRI_NODE(ctx, target).tag != IRI_GEN::EnvBinding)
      return state;

    IRID src = IRI_GEN::EnvReadSEXP(rval, ctx).getArg_Obj();
    if (IRI_NODE(ctx, src).tag != IRI_GEN::EnvBinding || src == target ||
        !state.contains(src) || !state.contains(target))
      return state;

    // A binding a nested closure or eval can reach may be written from outside
    // this CFG, where no kill can see it.
    auto &iris = ctx.iris;
    if (IRI_BINDING(iris, target).isCaptured() ||
        IRI_BINDING(iris, src).isCaptured() ||
        IRI_BINDING(iris, target).isEvalTainted() ||
        IRI_BINDING(iris, src).isEvalTainted())
      return state;

    // Shorten the chain: after a = b, the write c = a records c = b.
    CopyLattice fact = state.getLattice(src);
    IRID mirror = fact.kind == CopyLattice::COPY ? fact.src : src;
    return state.setLattice(target, CopyLattice(CopyLattice::COPY, mirror));
  }
};

//
// Machinery
//
class CopiesAtStmtResult : public DataflowResultConcept {
private:
  std::shared_ptr<CopiesAtStmtTransfer> transfer;
  std::shared_ptr<DataflowSolver<CopyState>> solver;

public:
  CopiesAtStmtResult() = default;
  CopiesAtStmtResult(std::shared_ptr<CopiesAtStmtTransfer> t,
                     std::shared_ptr<DataflowSolver<CopyState>> s)
      : transfer(std::move(t)), solver(std::move(s)) {}

  std::string getAnalysisName() const override { return "CopiesAtStmt"; }

  const CopyState &getStateBeforeStatement(BBIDX block,
                                           size_t stmtOffset) const {
    return solver->getStateBeforeStatement(block, stmtOffset);
  }
};

struct CopiesAtStmt {
  static inline char ID = 0;
  using Result = CopiesAtStmtResult;

  CopiesAtStmtResult run(IRICFG &cfg, AnalysisManager &am) {
    auto transfer = std::make_shared<CopiesAtStmtTransfer>();
    auto solver = std::make_shared<DataflowSolver<CopyState>>(cfg, *transfer);

    SCPIDX headScope = cfg.at(cfg.entry).SCOPE;
    auto bindings = cfg.ctx.iris->getEnvBindingsInClosure(headScope);

    solver->run(CopyState::over(bindings, CopyLattice(CopyLattice::NOEFFECT)),
                CopyState::over(bindings, CopyLattice(CopyLattice::UNKNOWN)));

    return CopiesAtStmtResult(std::move(transfer), std::move(solver));
  }
};

} // namespace IRI_STRUCTURAL
