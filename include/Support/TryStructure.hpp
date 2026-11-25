#pragma once
#include "Storage/Config.h"
#include <memory>
#include <unordered_map>

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_STRUCTURAL {

using namespace IRI_STORAGE;

class IRIS;

//
// Temporary Exception redirect structure for core passes
//
class TryStructure {
public:
  TryStructure(
      IRIS &,
      std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>> &);

  BBIDX getExceptionTargetForScope(SCPIDX) const;
  BBIDX getFinalizerRetBBIDX(BBIDX finalizerIDX) const;

private:
  IRIS &iris;

  // try/ud-catch scope -> the BB its throw redirects to
  std::unordered_map<SCPIDX, BBIDX> exceptionEdgeRedirect;

  // finalizer BB -> the BB its body returns to
  std::unordered_map<BBIDX, BBIDX> finalizerRetMap;
};

} // namespace IRI_STRUCTURAL
