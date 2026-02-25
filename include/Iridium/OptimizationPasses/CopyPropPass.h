#pragma once

#include "Iridium/OptimizationPasses/BBPass.h"
#include "Iridium/Analysis/Domains/CopyPropInfo.h"

class CopyPropPass : public BBPass
{
  static std::shared_ptr<EnvBindingSEXP> resolve(const std::shared_ptr<EnvBindingSEXP> &start, const CopyPropInfo &val);
  static bool patchExpr(IRISEXP curr, const CopyPropInfo &val);
  static bool Transform(std::shared_ptr<BBSEXP> &bb, const CopyPropInfo &inData);

public:
  std::string name() const override final;
  virtual bool run(BBContainerView &bb, FileView &fileView, AnalysisManager &AM) override final;
};
