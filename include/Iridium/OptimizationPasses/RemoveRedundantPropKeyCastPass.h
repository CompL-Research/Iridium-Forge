#pragma once

#include "Iridium/OptimizationPasses/BBPass.h"
#include "Iridium/Analysis/Domains/SetSafePropKeyAccesses.h"

class RemoveRedundantPropKeyCastPass : public BBPass
{
  static bool patchExpr(IRISEXP curr, const SetSafePropKeyAccesses &flowVal);
  static bool Transform(std::shared_ptr<BBSEXP> &bb,
                        const SetSafePropKeyAccesses &inData);

public:
  std::string name() const override final;
  virtual bool run(BBContainerView &bb,
                   FileView &fileView,
                   AnalysisManager &AM) override final;
};