// #pragma once
#include "Generated/IridiumTypes.h"
#include "Support/IndexedIterator.hpp"

namespace IRI_STRUCTURAL {
struct BBContainerSupport : IRI_GEN::BBContainerSEXP {
public:
  BBContainerSupport(IRI_GEN::IRID n, IRI_GEN::IridiumPool &p)
      : IRI_GEN::BBContainerSEXP(n, p) {}

  IndexedIterator<std::vector<IRI_GEN::IRID>> bbs() const;

  IRI_STORAGE::IRID getBBByIDX(double);
};
} // namespace IRI_STRUCTURAL
