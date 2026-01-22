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
        // WIP, this needs some on the merge... currently under specific circumstances
        // when EPROP and WBR passes are disabled, this wont terminate
        //  should be an easy fix leaving this for someone else to handle...
        next.dfv.store[lVal] = PropKeyLatticeValue::generate(envWrite->getRVal(), next.dfv);
        // auto newVal = PropKeyLatticeValue::generate(envWrite->getRVal(), next.dfv);
        // next.dfv.store[lVal] = next.dfv.store[lVal].merge(newVal);
      }
    }
    callback(idx++, next);
  }

  return next;
}
