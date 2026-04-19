
#pragma once
#include "Generated/IridiumTypes.h"
#include "Support/IndexedIterator.hpp"
#include "Support/IndexedIterator.hpp"

namespace IRI_STRUCTURAL {
class BindingsSupport : IRI_GEN::BindingsSEXP {
public:
  BindingsSupport(IRI_GEN::IRID n, IRI_GEN::IridiumPool &p)
      : IRI_GEN::BindingsSEXP(n, p) {}

  IndexedIterator<std::vector<IRI_GEN::IRID>> localBindings() const;
  IndexedIterator<std::vector<IRI_GEN::IRID>> remoteBindings() const;
  IndexedIterator<std::vector<IRI_GEN::IRID>> lambdas() const;
};
} // namespace IRI_STRUCTURAL
