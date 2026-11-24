#pragma once
#include "Generated/IridiumTypes.h"
#include "Support/IRICFG.hpp"
#include "Support/IndexedIterator.hpp"

namespace IRI_STRUCTURAL {
struct FileSupport : IRI_GEN::FileSEXP {
public:
  FileSupport(IRI_GEN::IRID n, IRI_GEN::IRIContext &p)
      : IRI_GEN::FileSEXP(n, p) {}

  IndexedIterator<std::vector<IRI_GEN::IRID>> containers() const;

  IRI_STORAGE::IRID moduleRequests() const;

  IRI_STORAGE::IRID staticImports() const;

  IRI_STORAGE::IRID staticExports() const;

  IRI_STORAGE::IRID staticStarExports() const;

};
} // namespace IRI_STRUCTURAL
