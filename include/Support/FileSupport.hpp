#pragma once
#include "Generated/IridiumTypes.h"
#include "Support/IndexedIterator.hpp"

namespace IRI_STRUCTURAL {
struct FileSupport : IRI_GEN::FileSEXP {
public:
  FileSupport(IRI_GEN::IRID n, IRI_GEN::IridiumPool &p)
      : IRI_GEN::FileSEXP(n, p) {}

  IndexedIterator<std::vector<IRI_GEN::IRID>> containers() const;

  IndexedIterator<std::vector<IRI_GEN::IRID>> moduleRequests() const;

  IndexedIterator<std::vector<IRI_GEN::IRID>> staticImports() const;

  IndexedIterator<std::vector<IRI_GEN::IRID>> staticExports() const;

  IndexedIterator<std::vector<IRI_GEN::IRID>> staticStarExports() const;

  IRI_GEN::IRID operator[](double id) const;
};
} // namespace IRI_STRUCTURAL
