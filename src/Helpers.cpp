#include "Helpers.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"
#include <stdexcept>

namespace IRI_HELPERS {

auto findParentClosureScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) -> double {
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " +
                             std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  IRI_GEN::BBSEXP startBB(buildContext->BB.at(0), pool);

  if (startBB.hasClosureBoundary() || startBB.hasTopLevel())
    return startingScope;
  return findParentClosureScope(pool, buildContext->parent,
                                iridiumBuildContext);
}

auto getLexicalScope(
    IRI_STORAGE::IridiumPool &pool, double startingScope,
    std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>>
        &iridiumBuildContext) -> double {
  if (startingScope == -1)
    return -1;
  if (iridiumBuildContext.find(startingScope) == iridiumBuildContext.end())
    throw std::runtime_error("build context not found for scope: " +
                             std::to_string(startingScope));

  auto &buildContext = iridiumBuildContext[startingScope];
  return buildContext->parent;
}

} // namespace IRI_HELPERS
