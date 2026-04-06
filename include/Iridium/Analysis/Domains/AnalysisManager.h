#pragma once

#include <unordered_map>

#include "Iridium/Analysis/Domains/ContainerAnalysisCache.h"
#include "Iridium/Structure/BBContainerView.h"
#include "Iridium/Analysis/DataflowSolver.h"

class AnalysisManager
{
private:
    std::unordered_map<BBContainerView *, ContainerAnalysisCache> cacheMap;

    ContainerAnalysisCache &getCache(BBContainerView &bb)
    {
        auto [it, _] = cacheMap.try_emplace(&bb);
        return it->second;
    }

public:
    // ========================
    // Invalidation
    // ========================

    void invalidate(BBContainerView &bb)
    {
        auto it = cacheMap.find(&bb);
        if (it != cacheMap.end())
            it->second.clear();
    }

    void clearAll()
    {
        cacheMap.clear();
    }

    // ========================
    // Structural Analyses
    // ========================

    const CapturedBindingsResult &getCapturedBindings(BBContainerView &bb)
    {
        auto &cache = getCache(bb);

        if (!cache.captured)
            cache.captured = bb.getCapturedStackBindings();

        return *cache.captured;
    }

    const UncapturedBindingsResult &getUncapturedBindings(BBContainerView &bb)
    {
        auto &cache = getCache(bb);

        if (!cache.uncaptured)
            cache.uncaptured = bb.getUncapturedStackBindings();

        return *cache.uncaptured;
    }

    const AllBindingsResult &getAllBindings(BBContainerView &bb)
    {
        auto &cache = getCache(bb);

        if (!cache.allBindings)
            cache.allBindings = bb.getAllStackBindings();

        return *cache.allBindings;
    }

    // ========================
    // Dataflow Analyses
    // ========================

    const ConstantsResult &getConstants(BBContainerView &bb)
    {
        auto &cache = getCache(bb);

        if (!cache.constants)
        {
            const auto &captured = getCapturedBindings(bb);
            ConstantsAtStmt::blacklist = captured;

            DataflowSolver<ConstantsAtStmt> solver(
                bb.cfgManager,
                true,
                []()
                { return ConstantsAtStmt::bottom(); });

            cache.constants = solver.run(ConstantsAtStmt::boundary());
        }

        return *cache.constants;
    }

    const CopyPropResult &getCopyProp(BBContainerView &bb)
    {
        auto &cache = getCache(bb);

        if (!cache.copyProp)
        {
            const auto &captured = getCapturedBindings(bb);
            CopyPropInfo::blacklist = captured;

            DataflowSolver<CopyPropInfo> solver(
                bb.cfgManager,
                true,
                []()
                { return CopyPropInfo(); });

            cache.copyProp = solver.run(CopyPropInfo());
        }

        return *cache.copyProp;
    }

    const LivenessResult &getLiveness(BBContainerView &bb)
    {
        auto &cache = getCache(bb);

        if (!cache.liveness)
        {
            const auto &captured = getCapturedBindings(bb);
            Liveness::blacklist = captured;

            DataflowSolver<Liveness> solver(
                bb.cfgManager,
                false,
                []()
                { return Liveness::bottom(); });

            cache.liveness = solver.run(Liveness::boundary(captured));
        }

        return *cache.liveness;
    }

    const TDZAResult &getTDZA(BBContainerView &bb)
    {
        auto &cache = getCache(bb);

        if (!cache.tdza)
        {
            const auto &all = getAllBindings(bb);

            DataflowSolver<TDZA> solver(
                bb.cfgManager,
                true,
                [&]()
                { return TDZA::bottom(all); });

            cache.tdza = solver.run(TDZA::boundary(all));
        }

        return *cache.tdza;
    }

    const EffectResult &getEffects(BBContainerView &bb)
    {
        auto &cache = getCache(bb);

        if (!cache.effects)
        {
            const auto &captured = getCapturedBindings(bb);
            EffectAtStmt::blacklist = captured;

            DataflowSolver<EffectAtStmt> solver(
                bb.cfgManager,
                true,
                []()
                { return EffectAtStmt(); });

            cache.effects = solver.run(EffectAtStmt());
        }

        return *cache.effects;
    }

    const SafePropKeyResult &getSafePropKey(BBContainerView &bb)
    {
        auto &cache = getCache(bb);

        if (!cache.safePropKey)
        {
            const auto &uncaptured = getUncapturedBindings(bb);

            DataflowSolver<SetSafePropKeyAccesses> solver(
                bb.cfgManager,
                true,
                [&]()
                { return SetSafePropKeyAccesses::bottom(uncaptured); });

            cache.safePropKey = solver.run(
                SetSafePropKeyAccesses::boundary(uncaptured));
        }

        return *cache.safePropKey;
    }
};
