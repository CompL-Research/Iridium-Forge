#pragma once

#include "Iridium/OptimizationPasses/BBPass.h"
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/Analysis/Domains/EffectAtStmt.h"

class PropEffectsPass : public BBPass
{
  static bool patchExprOuter(IRISEXP expr,
                             std::set<IRISEXP> &killset,
                             const EffectAtStmt &val);

  static bool Transform(std::shared_ptr<BBSEXP> &bb,
                        const Liveness &livenessAfterStmt,
                        const EffectAtStmt &effectAtStmt);

public:
  std::string name() const override final;
  virtual bool run(BBContainerView &bb,
                   FileView &fileView,
                   AnalysisManager &AM) override final;
};