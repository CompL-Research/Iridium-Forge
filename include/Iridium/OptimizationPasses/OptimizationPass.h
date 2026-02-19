#ifndef OPTIMIZATION_PASS_H
#define OPTIMIZATION_PASS_H

#include <unordered_map>

#include "Iridium/Structure/BBContainerView.h"
#include "Iridium/Structure/FileView.h"
class OptimizationPass
{
public:
    virtual const char *name() const = 0;

    virtual bool isEnabled() const { return true; }

    // Return true if IR changed (can return true always initially)
    virtual bool run(
        BBContainerView &bb,
        FileView &fileView,
        std::unordered_map<int, IRIBUILDCONTEXT> &ctx) = 0;

    virtual ~OptimizationPass() = default;
};

#endif
