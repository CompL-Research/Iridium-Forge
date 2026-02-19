#pragma once
#include "Iridium/Analysis/OptimizationPasses/OptimizationPass.h"
#include "Iridium/Analysis/Domains/ConstantsAtStmt.h"

class ConstantPropPass : public OptimizationPass
{
public:
  const char *name() const override { return "ConstantProp"; }

  bool isEnabled() const override
  {
    return !getenv("NO_CONSTPROP");
  }

  bool run(
      BBContainerView &bb,
      FileView &,
      std::unordered_map<int, IRIBUILDCONTEXT> &) override
  {
    auto captured = bb.getCapturedStackBindings();
    ConstantsAtStmt::blacklist = captured;

    DataflowSolver<ConstantsAtStmt> solver(
        bb.cfgManager,
        true,
        []
        { return ConstantsAtStmt::bottom(); });

    for (auto &e : solver.run(ConstantsAtStmt::boundary()))
    {
      ConstantProp::Transform(bb.cfgManager.cfg[e.first], e.second);
    }

    return true; // safe default
  }
};
