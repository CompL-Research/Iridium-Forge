#include "Iridium/OptimizationPasses/RemoveRedundantPropKeyCastPass.h"

// --------------------------------------------
// Recursively update SAFE flags
// --------------------------------------------
bool RemoveRedundantPropKeyCastPass::patchExpr(
    IRISEXP curr,
    const SetSafePropKeyAccesses &flowVal)
{
    bool changed = false;

    if (auto computedFieldRead =
            std::dynamic_pointer_cast<JSComputedFieldReadSEXP>(curr))
    {
        bool shouldBeSafe =
            PropKeyLatticeValue::isSafe(
                computedFieldRead->getField(),
                flowVal.dfv);

        if (shouldBeSafe && !computedFieldRead->hasSAFE())
        {
            computedFieldRead->setSAFE();
            changed = true;
        }
        else if (!shouldBeSafe && computedFieldRead->hasSAFE())
        {
            computedFieldRead->unsetSAFE();
            changed = true;
        }
    }

    if (auto computedFieldWrite =
            std::dynamic_pointer_cast<JSComputedFieldWriteSEXP>(curr))
    {
        bool shouldBeSafe =
            PropKeyLatticeValue::isSafe(
                computedFieldWrite->getField(),
                flowVal.dfv);

        if (shouldBeSafe && !computedFieldWrite->hasSAFE())
        {
            computedFieldWrite->setSAFE();
            changed = true;
        }
        else if (!shouldBeSafe && computedFieldWrite->hasSAFE())
        {
            computedFieldWrite->unsetSAFE();
            changed = true;
        }
    }

    // Recurse
    for (auto &e : curr->args)
        changed |= patchExpr(e, flowVal);

    return changed;
}

// --------------------------------------------
// Transform one basic block
// --------------------------------------------
bool RemoveRedundantPropKeyCastPass::Transform(
    std::shared_ptr<BBSEXP> &bb,
    const SetSafePropKeyAccesses &inData)
{
    bool changed = false;

    inData.iter(bb, [&](size_t idx, const SetSafePropKeyAccesses &val)
                {
        // Fast path: no safe info
        if (val.dfv.store.empty())
            return;

        changed |= patchExpr(bb->args.at(idx), val); });

    return changed;
}

// --------------------------------------------
// Pass entry
// --------------------------------------------
bool RemoveRedundantPropKeyCastPass::run(
    BBContainerView &bb,
    FileView & /*fileView*/,
    AnalysisManager &AM)
{
    bool changed = false;

    const auto &safeInfo = AM.getSafePropKey(bb);

    for (const auto &[vertex, state] : safeInfo)
    {
        if (state.dfv.store.empty())
            continue;

        auto currBB = bb.cfgManager.cfg[vertex];
        changed |= Transform(currBB, state);
    }

    return changed;
}
