#pragma once

#include "Iridium/OptimizationPasses/BBPass.h"

class ConstantPropPass : public BBPass
{
    static std::shared_ptr<IridiumSEXP> makeConstant(const ConstantLatticeValue &val);
    static bool patchExpr(IRISEXP expr, IRISEXP binding, const ConstantLatticeValue &val);
    static bool Transform(std::shared_ptr<BBSEXP> &bb, const ConstantsAtStmt &inData);

public:
    std::string name() const override final;
    virtual bool run(BBContainerView &bb, FileView &fileView, AnalysisManager &AM) override final;
};
