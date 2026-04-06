#pragma once
#include "Iridium/OptimizationPasses/BBPass.h"

class ReduceComputedFieldOpsPass : public BBPass
{
    static bool patchExpr(IRISEXP curr);
    static bool runOnContainer(BBContainerView &bb);

public:
    std::string name() const override;

    bool run(BBContainerView &bb,
             FileView &fileView,
             AnalysisManager &AM) override final;
};