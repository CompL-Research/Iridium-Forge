#pragma once
#include "Storage/Config.h"
#include "Storage/StringPool.h"
#include <functional>
#include <memory>
#include <unordered_map>

namespace IRI_STORAGE {
class IridiumPool;
}

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_STRUCTURAL {
class FileSupport;
class BindingsSupport;
} // namespace IRI_STRUCTURAL

namespace IRI_HELPERS {

double findVARHoistingScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

double findParentClosureScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

double getLexicalScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

IRI_STORAGE::IRID getTopLevelContainer(IRI_STORAGE::IridiumPool &pool,
                                       IRI_STORAGE::IRID fileSEXP);
double getTopLevelScope(IRI_STORAGE::IridiumPool &pool,
                        IRI_STORAGE::IRID fileSEXP);

IRI_STORAGE::IRID resolveRemoteBinding(IRI_STORAGE::IridiumPool &pool,
                                       IRI_STORAGE::IRID rbinID);

bool hasNodeWithPredicate(IRI_STORAGE::IRID id, IRI_STORAGE::IridiumPool *pool,
                          std::function<bool(IRI_STORAGE::IRID)> pred);

void countNodeOccurenceWithPredicate(
    IRI_STORAGE::IRID id, IRI_STORAGE::IridiumPool *pool,
    std::function<bool(IRI_STORAGE::IRID)> pred, size_t &count);

IRI_STORAGE::IRID
createNoASWResolveEnvBindingSEXP(IRI_STORAGE::IridiumPool &pool, StringID s);
IRI_STORAGE::IRID createUnsafeEnvReadSEXP(IRI_STORAGE::IridiumPool &pool,
                                          StringID s);

} // namespace IRI_HELPERS
