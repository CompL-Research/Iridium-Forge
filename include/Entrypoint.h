#pragma once
#include "Storage/Config.h"
#include <memory>
#include <unordered_map>

namespace IRI_STORAGE {
class IRIContext;
}

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_ENTRY {
IRI_STORAGE::IRID sharedEntrypoint(
    IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::IRID file,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);
} // namespace IRI_ENTRY
