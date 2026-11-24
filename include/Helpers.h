#pragma once
#include "Storage/Config.h"
#include "Storage/StringPool.h"
#include "Support/IRICFG.hpp"
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

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

IRI_STORAGE::SCPIDX findVARHoistingScope(
    IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::SCPIDX startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

IRI_STORAGE::SCPIDX findParentClosureScope(
    IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::SCPIDX startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

IRI_STORAGE::SCPIDX getLexicalScope(
    IRI_STORAGE::IRIContext &ctx, IRI_STORAGE::SCPIDX startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext);

IRI_STORAGE::IRID getTopLevelContainer(IRI_STORAGE::IRIContext &ctx,
                                       IRI_STORAGE::IRID fileSEXP);

// Where a pass prepends the statements it hoists to "function entry". A module's
// entry block only branches between its declaration pass and its evaluation
// pass, so its prologue belongs at the head of the declaration block instead:
// that pass initializes the exported bindings and then stops, and the evaluation
// pass must not create the closures a second time.
IRI_STORAGE::IRID
getHoistTargetBB(IRI_STORAGE::IRIContext &ctx,
                 const std::vector<IRI_STORAGE::IRID> &bbs,
                 IRI_STORAGE::IRID startBBID);
IRI_STORAGE::SCPIDX getTopLevelScope(IRI_STORAGE::IRIContext &ctx,
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
