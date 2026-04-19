
#pragma once
#include "Generated/IridiumTypes.h"
#include "Storage/StringPool.h"
#include "Support/IndexedIterator.hpp"

namespace IRI_PARSE {
class IridiumBuildContext;
}

namespace IRI_STRUCTURAL {

struct BindingsSupport : IRI_GEN::BindingsSEXP {
public:
  BindingsSupport(IRI_GEN::IRID n, IRI_GEN::IridiumPool &p)
      : IRI_GEN::BindingsSEXP(n, p) {}

  IndexedIterator<std::vector<IRI_GEN::IRID>> localBindings() const;
  IndexedIterator<std::vector<IRI_GEN::IRID>> remoteBindings() const;
  IndexedIterator<std::vector<IRI_GEN::IRID>> lambdas() const;

  void balance(
      std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>> &);

  std::optional<IRI_GEN::IRID> getBinding(
      std::unordered_map<int, std::shared_ptr<IRI_PARSE::IridiumBuildContext>> &,
      StringID name, double lookupScope);
};
} // namespace IRI_STRUCTURAL
