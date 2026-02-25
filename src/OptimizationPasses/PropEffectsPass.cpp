#include "Iridium/OptimizationPasses/PropEffectsPass.h"

std::string PropEffectsPass::name() const
{
    return "PropEffects";
}

// --- Transform one basic block ---
bool PropEffectsPass::Transform(std::shared_ptr<BBSEXP> &bb,
                                const Liveness &livenessAfterStmt,
                                const EffectAtStmt &effectAtStmt)
{
    bool changed = false;
    std::set<IRISEXP> killset;

    // Materialize per-statement info (same as your original design)
    std::unordered_map<size_t, Liveness> livenessInfo;
    std::unordered_map<size_t, EffectAtStmt> effectInfo;

    livenessAfterStmt.iter(bb, [&](size_t idx, const Liveness &val)
                           { livenessInfo[idx] = val; });

    effectAtStmt.iter(bb, [&](size_t idx, const EffectAtStmt &val)
                      { effectInfo[idx] = val; });

    // Propagate effects forward
    for (size_t idx = 0; idx < bb->args.size(); idx++)
    {
        auto &stmt = bb->args[idx];
        const auto &currEffect = effectInfo[idx];
        const auto &liveAfter = livenessInfo[idx];

        if (currEffect.validEffect &&
            liveAfter.dfv.count(currEffect.store) == 0)
        {
            // This may modify later expressions and add to killset
            patchExprOuter(stmt, killset, currEffect);
        }
    }

    // Remove killed statements
    if (!killset.empty())
    {
        auto &stmts = bb->args;

        stmts.erase(
            std::remove_if(stmts.begin(), stmts.end(),
                           [&](const IRISEXP &s)
                           {
                               return killset.count(s) > 0;
                           }),
            stmts.end());

        changed = true;
    }

    return changed;
}

bool PropEffectsPass::run(BBContainerView &bb, FileView & /*fileView*/, AnalysisManager &AM)
{
    bool changed = false;

    // Analyses (lazy)
    const auto &captured = AM.getCapturedBindings(bb);
    Liveness::blacklist = captured;
    EffectAtStmt::blacklist = captured;

    const auto &liveness = AM.getLiveness(bb);
    const auto &effects = AM.getEffects(bb);

    for (const auto &[vertex, effectState] : effects)
    {
        auto currBB = bb.cfgManager.cfg[vertex];
        changed |= Transform(currBB,
                             liveness.at(vertex),
                             effectState);
    }

    return changed;
}