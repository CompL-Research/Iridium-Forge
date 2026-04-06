#pragma once

#include <string>

#include "Iridium/Structure/BBContainerView.h"
#include "Iridium/Structure/FileView.h"
#include "Iridium/Analysis/Domains/AnalysisManager.h"

class BBPass
{
public:
    virtual ~BBPass() = default;

    // Run pass on one container
    // Return true if IR changed
    virtual bool run(BBContainerView &bb, FileView &fileView, AnalysisManager &AM) = 0;

    // For debugging / instrumentation
    virtual std::string name() const = 0;
};
