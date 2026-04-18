#pragma once
#include "Storage/Config.h"
#include <memory>
#include <unordered_map>

namespace IRI_STORAGE {
class IridiumPool;
}

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_HELPERS {

double findParentClosureScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

double getLexicalScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

IRI_STORAGE::IRID getTopLevelContainer(IRI_STORAGE::IridiumPool &pool, IRI_STORAGE::IRID fileSEXP);

} // namespace IRI_HELPERS
