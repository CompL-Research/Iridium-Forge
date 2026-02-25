#pragma once

#include <optional>

#include "Iridium/Analysis/Domains/AnalysisResults.h"

struct ContainerAnalysisCache
{
    // Dataflow
    std::optional<ConstantsResult> constants;
    std::optional<CopyPropResult> copyProp;
    std::optional<TDZAResult> tdza;
    std::optional<LivenessResult> liveness;
    std::optional<EffectResult> effects;
    std::optional<SafePropKeyResult> safePropKey;

    // Structural
    std::optional<CapturedBindingsResult> captured;
    std::optional<UncapturedBindingsResult> uncaptured;
    std::optional<AllBindingsResult> allBindings;

    void clear()
    {
        constants.reset();
        copyProp.reset();
        tdza.reset();
        liveness.reset();
        effects.reset();
        safePropKey.reset();
        captured.reset();
        uncaptured.reset();
        allBindings.reset();
    }
};
