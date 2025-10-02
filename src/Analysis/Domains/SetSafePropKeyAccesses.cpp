#include "Iridium/Analysis/Domains/SetSafePropKeyAccesses.h"

std::set<std::shared_ptr<EnvBindingSEXP>> SetSafePropKeyAccesses::blacklist;

SetSafePropKeyAccesses SetSafePropKeyAccesses::transfer(const std::shared_ptr<BBSEXP> &bb) const
{
  return iter(bb, [&](size_t idx, SetSafePropKeyAccesses val) {});
}



SetSafePropKeyAccesses SetSafePropKeyAccesses::iter(const std::shared_ptr<BBSEXP> &bb, std::function<void(size_t, SetSafePropKeyAccesses)> callback) const
{
  auto next = this->clone();

  size_t idx = 0;
  for (const auto &stmt : bb->args)
  {
    if (auto envWrite = std::dynamic_pointer_cast<EnvWriteSEXP>(stmt))
    {
      if (auto lVal = std::dynamic_pointer_cast<EnvBindingSEXP>(envWrite->getLValTarget()))
      {
        if (blacklist.count(lVal) > 0) continue;
        next.dfv.store[lVal] = PropKeyLatticeValue::generate(envWrite->getRVal(), next.dfv);
      }
    }
    callback(idx++, next);
  }

  return next;
}
