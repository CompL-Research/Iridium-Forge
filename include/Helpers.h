#pragma once
#include "Storage/Config.h"
#include "Storage/StringPool.h"
#include <functional>
#include <memory>
#include <unordered_map>

namespace IRI_STORAGE {
class IRIContext;
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
    IRI_STORAGE::IRIContext &ctx, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

double findParentClosureScope(
    IRI_STORAGE::IRIContext &ctx, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

double getLexicalScope(
    IRI_STORAGE::IRIContext &ctx, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

IRI_STORAGE::IRID getTopLevelContainer(IRI_STORAGE::IRIContext &ctx,
                                       IRI_STORAGE::IRID fileSEXP);
double getTopLevelScope(IRI_STORAGE::IRIContext &ctx,
                        IRI_STORAGE::IRID fileSEXP);

IRI_STORAGE::IRID resolveRemoteBinding(IRI_STORAGE::IRIContext &ctx,
                                       IRI_STORAGE::IRID rbinID);

bool hasNodeWithPredicate(IRI_STORAGE::IRID id, IRI_STORAGE::IRIContext *ctx,
                          std::function<bool(IRI_STORAGE::IRID)> pred);

void countNodeOccurenceWithPredicate(
    IRI_STORAGE::IRID id, IRI_STORAGE::IRIContext *ctx,
    std::function<bool(IRI_STORAGE::IRID)> pred, size_t &count);

IRI_STORAGE::IRID
createNoASWResolveEnvBindingSEXP(IRI_STORAGE::IRIContext &ctx, StringID s);
IRI_STORAGE::IRID createUnsafeEnvReadSEXP(IRI_STORAGE::IRIContext &ctx,
                                          StringID s);

} // namespace IRI_HELPERS
