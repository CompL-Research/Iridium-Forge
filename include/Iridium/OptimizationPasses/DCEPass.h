#pragma once
#include "Iridium/OptimizationPasses/BBPass.h"
#include "Iridium/Analysis/Domains/Liveness.h"
#include "Iridium/CorePasses/3_filterNops.h"

class DCEPass : public BBPass
{

  static bool maybeSideEffect(IRISEXP rVAL);

  static IRISEXP patchExpr(IRISEXP curr, const Liveness &val);

  static bool Transform(std::shared_ptr<BBSEXP> &bb, const Liveness &inData);
public:
  virtual std::string name() const override final;
  virtual bool run(BBContainerView &bb, FileView & /*fileView*/, AnalysisManager &AM) override final;
};