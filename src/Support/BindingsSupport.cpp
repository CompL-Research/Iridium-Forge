#include "Support/BindingsSupport.hpp"
#include "Generated/IridiumTypes.h"
#include "Support/IndexedIterator.hpp"
#include <stdexcept>

namespace IRI_STRUCTURAL {
using namespace IRI_GEN;
using namespace IRI_STORAGE;
using ITR_RET = IndexedIterator<std::vector<IRID>>;

ITR_RET BindingsSupport::localBindings() const {
  BindingsSEXP bSEXP(id, *pool);
  IRID bindingsID = bSEXP.getArg_LocalBindings();
  ListSEXP bindingsList(bindingsID, *pool);
  if (bindingsList.getTYPE() == pool->strings.intern("EnvBinding"))
    throw std::runtime_error(
        "Expected TYPE EnvBinding to be set on localBindings list");
  return IndexedIterator(std::move(pool->get_args(bindingsID)));
}

ITR_RET BindingsSupport::remoteBindings() const {
  BindingsSEXP bSEXP(id, *pool);
  IRID bindingsID = bSEXP.getArg_RemoteBindings();
  ListSEXP bindingsList(bindingsID, *pool);
  if (bindingsList.getTYPE() == pool->strings.intern("RemoteEnvBinding"))
    throw std::runtime_error(
        "Expected TYPE RemoteEnvBinding to be set on remoteBindings list");
  return IndexedIterator(std::move(pool->get_args(bindingsID)));
}

ITR_RET BindingsSupport::lambdas() const {
  BindingsSEXP bSEXP(id, *pool);
  IRID bindingsID = bSEXP.getArg_Lambdas();
  ListSEXP bindingsList(bindingsID, *pool);
  if (bindingsList.getTYPE() == pool->strings.intern("PoolBinding"))
    throw std::runtime_error(
        "Expected TYPE PoolBinding to be set on poolBindings list");
  return IndexedIterator(std::move(pool->get_args(bindingsID)));
}

} // namespace IRI_STRUCTURAL
