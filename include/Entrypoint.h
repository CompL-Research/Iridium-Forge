#pragma once
#include "IRIPerf.h"
#include "Storage/Config.h"
#include <memory>
#include <unordered_map>

namespace IRI_STORAGE {
class IridiumPool;
}

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_ENTRY {
IRI_STORAGE::IRID sharedEntrypoint(
    IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID file,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext,
    IRIPerf &iriPerf);
} // namespace IRI_ENTRY
