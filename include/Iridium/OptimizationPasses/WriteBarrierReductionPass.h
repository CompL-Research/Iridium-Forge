#pragma once
#include "Iridium/OptimizationPasses/BBPass.h"
#include "Iridium/Analysis/Domains/TDZA.h"
#include "Iridium/Structure/FileView.h"

class WriteBarrierReductionPass : public BBPass
{
  static bool predicateLambdaSEXP(const IRISEXP &ele);
  static bool markCapturedBindings(IRISEXP curr, const TDZA &inData);
  static bool patchExpr(IRISEXP curr, IRISEXP binding, const TDZLattice &latticeVal);
  static bool Transform(std::shared_ptr<BBSEXP> &bb, const TDZA &inData);

  static FileView *currFileView;

public:
  virtual std::string name() const override final;
  virtual bool run(BBContainerView &bb, FileView &fileView, AnalysisManager &AM) override final;
};
