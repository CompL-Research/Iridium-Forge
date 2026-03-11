#include "Iridium/OptimizationPasses/PropEffectsPass.h"

std::string PropEffectsPass::name() const
{
    return "PropEffects";
}

static void patchSafeEnvReads(IRISEXP expr,
                              std::set<IRISEXP> &killset,
                              const EffectAtStmt &val)
{
    for (size_t i = 0; i < expr->args.size(); i++)
    {
        auto &e = expr->args[i];
        if (auto node = std::dynamic_pointer_cast<EnvReadSEXP>(e))
        {
            if (node->getObj() == val.store)
            {
                e = val.effect;
                killset.insert(val.stmt);
                continue;
            }
        }

        patchSafeEnvReads(e, killset, val);
    }
}

static bool patchExprNew(IRISEXP expr,
                         std::set<IRISEXP> &killset,
                         const EffectAtStmt &val)
{
    bool changed = false;

    for (size_t i = 0; i < expr->args.size(); i++)
    {
        auto &arg = expr->args[i];

        if (auto read = std::dynamic_pointer_cast<EnvReadSEXP>(arg))
        {
            if (read->getObj() == val.store)
            {
                arg = val.effect;
                killset.insert(val.stmt);
                changed = true;
            }
        }
        else
        {
            changed |= patchExprNew(arg, killset, val);
        }
    }

    return changed;
}

bool PropEffectsPass::patchExprOuter(IRISEXP expr,
                                     std::set<IRISEXP> &killset,
                                     const EffectAtStmt &val)
{
    bool changed = false;

    // Count occurrences of the store in this statement
    auto numOccurences = countNode(expr, [&](IRISEXP n)
                                   {
        if (auto r = std::dynamic_pointer_cast<EnvReadSEXP>(n))
            return r->getObj() == val.store;
        return false; });

    // --- Case 1: Safe read duplication allowed ---
    if (auto read = std::dynamic_pointer_cast<EnvReadSEXP>(val.effect))
    {
        bool safe =
            read->hasSAFE() ||
            std::dynamic_pointer_cast<GlobalBindingSEXP>(read->getObj());

        if (safe)
        {
            patchSafeEnvReads(expr, killset, val);
            return true;
        }
    }

    // --- Case 2: Only propagate if used once ---
    if (numOccurences != 1)
        return false;

    // Handle outer statement kinds

    if (auto ifElse = std::dynamic_pointer_cast<IfElseJumpSEXP>(expr))
    {
        if (auto read = std::dynamic_pointer_cast<EnvReadSEXP>(ifElse->getTest()))
        {
            if (read->getObj() == val.store)
            {
                ifElse->setTest(val.effect);
                killset.insert(val.stmt);
                return true;
            }
        }
        changed |= patchExprNew(ifElse->getTest(), killset, val);
    }
    else if (auto ret = std::dynamic_pointer_cast<ReturnSEXP>(expr))
    {
        if (auto read = std::dynamic_pointer_cast<EnvReadSEXP>(ret->getObj()))
        {
            if (read->getObj() == val.store)
            {
                ret->setObj(val.effect);
                killset.insert(val.stmt);
                return true;
            }
        }
        changed |= patchExprNew(ret->getObj(), killset, val);
    }
    else if (auto thr = std::dynamic_pointer_cast<ThrowSEXP>(expr))
    {
        if (auto read = std::dynamic_pointer_cast<EnvReadSEXP>(thr->getThrowVal()))
        {
            if (read->getObj() == val.store)
            {
                thr->setThrowVal(val.effect);
                killset.insert(val.stmt);
                return true;
            }
        }
        changed |= patchExprNew(thr->getThrowVal(), killset, val);
    }
    else if (auto write = std::dynamic_pointer_cast<EnvWriteSEXP>(expr))
    {
        changed |= patchExprNew(write->getRVal(), killset, val);
    }
    else
    {
        // Generic fallback
        changed |= patchExprNew(expr, killset, val);
    }

    return changed;
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