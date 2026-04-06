#include "Iridium/OptimizationPasses/ConstantPropPass.h"

std::string ConstantPropPass::name() const
{
    return "ConstantProp";
}

std::shared_ptr<IridiumSEXP> ConstantPropPass::makeConstant(const ConstantLatticeValue &val)
{
    switch (val.kind)
    {
    case ConstantLatticeValue::Null:
        return std::make_shared<NullSEXP>(true); // vestigial

    case ConstantLatticeValue::Boolean:
    {
        auto booleanVal = std::dynamic_pointer_cast<BooleanSEXP>(val.value);
        assert(booleanVal);
        return std::make_shared<BooleanSEXP>(booleanVal->getIridiumPrimitive());
    }

    case ConstantLatticeValue::Number:
    {
        auto numberVal = std::dynamic_pointer_cast<NumberSEXP>(val.value);
        assert(numberVal);
        return std::make_shared<NumberSEXP>(numberVal->getIridiumPrimitive());
    }

    case ConstantLatticeValue::String:
    {
        auto stringVal = std::dynamic_pointer_cast<StringSEXP>(val.value);
        assert(stringVal);
        return std::make_shared<StringSEXP>(stringVal->getIridiumPrimitive());
    }

    case ConstantLatticeValue::JSBigInt:
    {
        auto bigIntVal = std::dynamic_pointer_cast<BitIntSEXP>(val.value);
        assert(bigIntVal);
        return std::make_shared<BitIntSEXP>(bigIntVal->getIridiumPrimitive());
    }

    case ConstantLatticeValue::NAC:
    case ConstantLatticeValue::NUBD:
    case ConstantLatticeValue::BOTTOM:
        throw std::runtime_error("Unreachable makeConstant");
        return nullptr; // not a constant we can propagate
    }
    throw std::runtime_error("Unreachable makeConstant");
    return nullptr; // defensive
}

bool ConstantPropPass::patchExpr(IRISEXP expr, IRISEXP binding, const ConstantLatticeValue &val)
{
    bool changed = false;

    for (size_t i = 0; i < expr->args.size(); i++)
    {
        auto &curr = expr->args[i];

        if (auto envReadNode = std::dynamic_pointer_cast<EnvReadSEXP>(curr))
        {
            if (envReadNode->getObj() == binding)
            {
                if (auto replacement = ConstantPropPass::makeConstant(val))
                {
                    expr->args[i] = replacement;
                    changed = true;
                    continue; // no need to recurse into replaced node
                }
            }
        }

        // Recurse
        changed |= ConstantPropPass::patchExpr(curr, binding, val);
    }

    return changed;
}

bool ConstantPropPass::Transform(std::shared_ptr<BBSEXP> &bb, const ConstantsAtStmt &inData)
{
    bool changed = false;

    inData.iter(bb, [&](size_t idx, const ConstantsAtStmt &val)
                {
                    if(val.dfv.store.empty()) {
                        return;
                    }
        
        for (auto &[binding, latticeVal] : val.dfv.store)
        {
            if (latticeVal.kind == ConstantLatticeValue::NAC ||
                latticeVal.kind == ConstantLatticeValue::NUBD)
                continue;

            if (latticeVal.kind == ConstantLatticeValue::BOTTOM)
                continue;

            changed |= ConstantPropPass::patchExpr(
                bb->args.at(idx),
                binding,
                latticeVal);
        } });

    return changed;
}

bool ConstantPropPass::run(BBContainerView &bb, FileView & /*fileView*/, AnalysisManager &AM)
{
    // if (std::getenv("NO_CONSTPROP"))
    // {
    //     return false;
    // }

    bool changed = false;

    // --- Analyses (lazy via manager) ---
    const auto &captured = AM.getCapturedBindings(bb);
    ConstantsAtStmt::blacklist = captured; // TODO: Fix this for multi-threaded accesses

    const auto &constants = AM.getConstants(bb);

    // --- Transform ---
    for (const auto &[vertex, state] : constants)
    {
        auto currBB = bb.cfgManager.cfg[vertex];
        changed |= ConstantPropPass::Transform(currBB, state);
    }

    return changed;
}
