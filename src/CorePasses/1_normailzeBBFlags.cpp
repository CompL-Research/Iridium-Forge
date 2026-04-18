#include "CorePasses.h"
#include "Generated/IridiumEnums.h"
#include "Generated/IridiumTypes.h"
#include "Parser/IridiumBuildContext.h"

namespace IRI_CORE_PASSES {
using namespace IRI_PARSE;
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using BUILD_CTX = std::unordered_map<int, std::shared_ptr<IridiumBuildContext>>;

inline IRI_FLAG getBBFlag(BBSEXP &b) {
  if (b.hasTopLevel())
    return IRI_FLAG::TopLevel;
  if (b.hasClosureBoundary())
    return IRI_FLAG::ClosureBoundary;
  if (b.hasLexical())
    return IRI_FLAG::Lexical;
  if (b.hasVARBoundary())
    return IRI_FLAG::VARBoundary;
  throw std::runtime_error("Failed to get a valid flag from a BBSEXP");
}

inline void setBBFlag(BBSEXP &b, IRI_FLAG flagToSet) {
  b.clearTopLevel();
  b.clearClosureBoundary();
  b.clearLexical();
  b.clearVARBoundary();
  if (flagToSet == IRI_GEN::TopLevel)
    return b.setTopLevel();
  if (flagToSet == IRI_GEN::ClosureBoundary)
    return b.setClosureBoundary();
  if (flagToSet == IRI_GEN::Lexical)
    return b.setLexical();
  if (flagToSet == IRI_GEN::VARBoundary)
    return b.setVARBoundary();
  throw std::runtime_error("Impossible case reached setBBFlag");
}

void _1_NBBF(IridiumPool &pool, IRID sexp, BUILD_CTX &iridiumBuildContext) {
  for (auto &e : iridiumBuildContext) {
    int scopeIdx = e.first;
    std::shared_ptr<IridiumBuildContext> buildContext = e.second;

    BBSEXP firstBB(buildContext->BB[0], pool);

    IRI_FLAG mainBBFlag = getBBFlag(firstBB);
    for (auto &b : buildContext->BB) {
      BBSEXP currBB(b, pool);
      setBBFlag(currBB, mainBBFlag);
    }
  }
}
}; // namespace IRI_CORE_PASSES
